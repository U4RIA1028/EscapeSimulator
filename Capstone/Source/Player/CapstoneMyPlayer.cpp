// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CapstoneMyPlayer.h"
#include "Capstone.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "../Object/Button/StartButton.h"
#include "../Object/ObjectBase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerBaseWidget.h"

ACapstoneMyPlayer::ACapstoneMyPlayer()
	: Super()
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//<Pickup>
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(RootComponent);
	FP_MuzzleLocation->SetWorldLocation(FVector(90.0f, 50.0f, 50.0f));

	HoldingComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HoldingComponent"));
	HoldingComponent->SetWorldLocation(FVector(0.0f, 0.0f, 0.0f));
	HoldingComponent->SetupAttachment(FP_MuzzleLocation);


	// Load the input mapping context from the reference path
	const FString ContextPath = TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Blueprints/Input/IMC_Default.IMC_Default'");
	DefaultMappingContext = Cast<UInputMappingContext>(StaticLoadObject(UInputMappingContext::StaticClass(), nullptr, *ContextPath));

	// Load the input action from the reference path
	const FString ActionPath = TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Input/Actions/IA_OpenMenu.IA_OpenMenu'");
	OpenMenuAction = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, *ActionPath));

	KeyCount = 0;
	ClearRoomCount = 0;
	bCanMove = true;
	bInspecting = false;
	bHoldingItem = false;
	Key = NULL;
	Object = NULL;
	for (int i = 0; i < 30; i++)
	{
		inventory[i] = 0;
	}
	//</Pickup>

}


void ACapstoneMyPlayer::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->bShowMouseCursor = false;

		// 히트 가능한 오브젝트 유형들
		TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
		TEnumAsByte<EObjectTypeQuery> WorldDynamic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic);

		ObjectTypes.Add(WorldStatic);
		ObjectTypes.Add(WorldDynamic);
	}
}

void ACapstoneMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PacketSendCountTime -= DeltaTime;

	if (PacketSendCountTime < 0)
	{
		// TODO
		SendSyncMove();

		PacketSendCountTime = MOVE_PACKET_SEND_DELAY;
	}

	{
		auto [Start, End] = CameraVector();

		AObjectBase* Base = FindActorBase(Start, End);
		if (Base)
		{
			PlayerWidget->FindActorVisible(true);
		}
		else
		{
			PlayerWidget->FindActorVisible(false);
		}
	}

}

void ACapstoneMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACapstoneMyPlayer::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ACapstoneMyPlayer::Stop);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACapstoneMyPlayer::Look);

		//Clicked
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &ACapstoneMyPlayer::Clicked);

		//Right
		EnhancedInputComponent->BindAction(TurnRightAction, ETriggerEvent::Started, this, &ACapstoneMyPlayer::TurnRight);

		//OpenMenu
		EnhancedInputComponent->BindAction(OpenMenuAction, ETriggerEvent::Started, this, &ACapstoneMyPlayer::OpenMenu);

	}
}

void ACapstoneMyPlayer::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (PlayerMoveInfo->move_type() != Protocol::MoveType::MOVE_RUN)
		SetMoveType(Protocol::MoveType::MOVE_RUN);

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ACapstoneMyPlayer::Stop(const FInputActionValue& Value)
{
	SetPlayerMoveInfo();

	SetMoveType(Protocol::MoveType::MOVE_IDLE);

	SendSyncMove();
}

void ACapstoneMyPlayer::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACapstoneMyPlayer::Clicked(const FInputActionValue& Value)
{
	// 카메라 위치 가져오기

	auto [Start, End] = CameraVector();

	AObjectBase* Base = FindActorBase(Start, End);

	if (Base != nullptr)
	{
		ObjectBaseClicked(Base);
	}

	if (bHoldingItem == false)
	{
		//<Pickup>
		StartDebug = CameraComponent->GetComponentLocation();
		ForwardVector = CameraComponent->GetForwardVector();
		EndDebug = ((ForwardVector * 200.0f) + StartDebug);

		if (GetWorld()->LineTraceSingleByChannel(Hit, StartDebug, EndDebug, ECC_Visibility, DefaultComponentQueryParams, DefaultResponseParams))
		{
			if (Hit.GetActor()->GetClass()->IsChildOf(AItemKey::StaticClass()))
			{
				Key = Cast<AItemKey>(Hit.GetActor());
			}
		}
		else
		{
			//후에 다시 바꿔야 할 코드
			Key = NULL;
			return;
		}

		if (GetWorld()->LineTraceSingleByChannel(Hit, StartDebug, EndDebug, ECC_Visibility, DefaultComponentQueryParams, DefaultResponseParams))
		{
			if (Hit.GetActor()->GetClass()->IsChildOf(APickupObject::StaticClass()))
			{
				Object = Cast<APickupObject>(Hit.GetActor());
			}
		}
		else
		{
			//후에 다시 바꿔야 할 코드
			Object = NULL;
			return;
		}
		if (Key == NULL && Object != NULL)
		{
			FName Socket(TEXT("hand_r_socket"));
			Object->PickupObject();
			Object->AttachToComponent(FP_MuzzleLocation, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket);
			bHoldingItem = true;
			return;
		}
		else if (Key != NULL && Object == NULL)
		{
			Key->Pickup();
			SetKeyCount(GetKeyCount() + 1);
			if (Key->GetKeyID())
			{
				SetInventory(Key->GetKeyID());
				SetInventory(Key->GetKeyID() + 1);
				SetInventory(Key->GetKeyID() + 2);
			}
			UE_LOG(LogTemp, Warning, TEXT("The Keys ID is %d"), Key->GetKeyID());
			Key = nullptr;
			return;
		}
		else if (Key != NULL && Object != NULL)
		{
			FName Socket(TEXT("hand_r_socket"));
			Object->PickupObject();
			Object->AttachToComponent(FP_MuzzleLocation, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket);
			bHoldingItem = true;
			Key->Pickup();
			SetKeyCount(GetKeyCount() + 1);
			if (Key->GetKeyID())
			{
				SetInventory(Key->GetKeyID());
				SetInventory(Key->GetKeyID() + 1);
				SetInventory(Key->GetKeyID() + 2);
			}
			UE_LOG(LogTemp, Warning, TEXT("The Keys ID is %d"), Key->GetKeyID());
			Key = nullptr;
		}
		else
		{
			return;
		}
	}
	else
	{
		Object->PickupObject();
		Object->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Object = NULL;
		bHoldingItem = false;
	}
	//</Pickup>
}

void ACapstoneMyPlayer::OpenMenu(const FInputActionValue& Value)
{
	// 게임 인스턴스 참조
	UCapstoneGameInstance* GameInstance = Cast<UCapstoneGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		// PlayerWidget 참조하여 MenuPanel 가시성 토글
		if (PlayerWidget.Get() != nullptr) // PlayerWidget이 nullptr이 아닌지 확인
		{
			PlayerWidget->ToggleMenuPanelVisibility();

			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				PlayerController->SetInputMode(FInputModeUIOnly());
				PlayerController->bShowMouseCursor = true;
			}
		}
	}
}


void ACapstoneMyPlayer::SetKeyCount(int num)
{
	KeyCount = num;
}

int ACapstoneMyPlayer::GetKeyCount()
{
	return KeyCount;
}

void ACapstoneMyPlayer::ResetRoomCount()
{
	ClearRoomCount = 0;
}

void ACapstoneMyPlayer::RoomCount()
{
	ClearRoomCount++;
}

int ACapstoneMyPlayer::GetRoomCount()
{
	return ClearRoomCount;
}

void ACapstoneMyPlayer::SetInventory(int id)
{
	for (int i = 0; i < 30; i++)
	{
		if (inventory[i] == 0)
		{
			inventory[i] = id;
			UE_LOG(LogTemp, Warning, TEXT("The inventorys ID is %d"), inventory[i]);
			break;
		}
		else
		{
			continue;
		}
	}
}

int ACapstoneMyPlayer::GetInventory(int i)
{
	return inventory[i];
}

void ACapstoneMyPlayer::TurnRight(const FInputActionValue& Value)
{
	IsRightOn = !IsRightOn;

	OnSpotLight(IsRightOn);

	{
		Protocol::C_ACTION_HAND_LIGHT ActionPkt;
		ActionPkt.set_player_id(PlayerInfo->object_id());
		ActionPkt.set_is_on_light(IsRightOn);

		SEND_PACKET(ActionPkt);
	}
}

void ACapstoneMyPlayer::InventoryClear()
{
	for (int i = 0; i < 30; i++)
	{
		inventory[i] = 0;
	}
}

void ACapstoneMyPlayer::SendSyncMove()
{
	Protocol::C_MOVE MovePkt;
	auto* MovePlayerInfo = MovePkt.mutable_player();
	MovePlayerInfo->CopyFrom(*PlayerInfo);

	SEND_PACKET(MovePkt);
}

void ACapstoneMyPlayer::ObjectBaseClicked(AObjectBase* Base)
{
	if (Base->OnClicked(this))
	{

	}
}

AObjectBase* ACapstoneMyPlayer::FindActorBase(FVector Start, FVector End)
{
	FHitResult OutHit;

	UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		ObjectTypes,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		OutHit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f);

	if (AObjectBase* Base = Cast<AObjectBase>(OutHit.GetActor()))
	{
		return Base;
	}

	return nullptr;
}

std::pair<FVector, FVector> ACapstoneMyPlayer::CameraVector()
{
	FVector Start = CameraComponent->GetComponentLocation();
	FVector End = Start + (CameraComponent->GetForwardVector() * 200.f);

	return { Start, End };
}
