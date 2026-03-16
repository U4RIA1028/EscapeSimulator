// Fill out your copyright notice in the Description page of Project Settings.


#include "CapstonePlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Controller.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ACapstonePlayer::ACapstonePlayer()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    //// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(40.f, 0.f, 70.f)); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	
	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLightComponent->Intensity = 30000.0f; // 밝기설정
	SpotLightComponent->AttenuationRadius = 5000.0f; // 범위설정
	SpotLightComponent->SetInnerConeAngle(30.0f); // 각도설정
	SpotLightComponent->SetOuterConeAngle(60.0f);
	SpotLightComponent->SetupAttachment(CameraComponent);

	ConstructorHelpers::FObjectFinder<UMaterialInstance> Meterial(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/SICKA_mansion/Materials/MI_FlashLight_Mask.MI_FlashLight_Mask'"));
	if (Meterial.Succeeded())
		SpotLightComponent->LightFunctionMaterial = Meterial.Object;

	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLightComponent->SetupAttachment(GetMesh());
	PointLightComponent->SetRelativeLocation(FVector(8.2f, 41.5f, 112.f));
	PointLightComponent->Intensity = 1.0f;
	PointLightComponent->AttenuationRadius = 850.f;
	PointLightComponent->bUseTemperature = true;
	PointLightComponent->Temperature = 8500.f;
	PointLightComponent->CastShadows = false;
	PointLightComponent->IntensityUnits = ELightUnits::Candelas;
	PointLightComponent->CastStaticShadows = false;
	PointLightComponent->CastDynamicShadows = false;

	PointLightComponent->SetVisibility(true);
	SpotLightComponent->SetVisibility(false);

	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 500.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	PlayerInfo = new Protocol::ObjectInfo();
	DestInfo = new Protocol::ObjectInfo();

	PlayerMoveInfo = PlayerInfo->mutable_move_info();
	DestMoveInfo = DestInfo->mutable_move_info();

	IsMine = false;
}

ACapstonePlayer::~ACapstonePlayer()
{
	delete PlayerInfo;
	PlayerInfo = nullptr;

	delete DestInfo;
	DestInfo = nullptr;
}

// Called when the game starts or when spawned
void ACapstonePlayer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACapstonePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 플레이어 카메라의 위치와 방향 가져오기
	FVector CameraLocation = CameraComponent->GetComponentLocation();
	FVector CameraForwardVector = CameraComponent->GetForwardVector();

	// SpotLightComponent 위치를 카메라 위치로 설정
	SpotLightComponent->SetWorldLocation(CameraLocation);

	// SpotLightComponent 방향을 카메라 방향으로 설정
	SpotLightComponent->SetWorldRotation(CameraForwardVector.Rotation());

	SetPlayerMoveInfo();

	if (IsMine == false)
	{
		switch (PlayerMoveInfo->move_type())
		{
		case Protocol::MoveType::MOVE_IDLE:
			MoveIdle(DeltaTime);
			break;
		case Protocol::MoveType::MOVE_RUN:
			MoveRun(DeltaTime);
			break;
		}
	}
}

// Called to bind functionality to input
void ACapstonePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACapstonePlayer::SetPlayerInfo(const Protocol::ObjectInfo& InputPlayerInfo)
{
	if (PlayerInfo == nullptr)
		return;

	PlayerInfo->CopyFrom(InputPlayerInfo);
	PlayerMoveInfo = PlayerInfo->mutable_move_info();
}

void ACapstonePlayer::SetDestInfo(const Protocol::ObjectInfo& InputPlayerInfo)
{
	if (DestInfo == nullptr)
		return;

	DestInfo->CopyFrom(InputPlayerInfo);
	DestMoveInfo = DestInfo->mutable_move_info();
	
	/*if (InputPlayerInfo.move_info().move_type() == Protocol::MoveType::MOVE_IDLE)
	{
		DestMoveInfo->set_yaw(InputPlayerInfo.move_info().yaw());
	}
	else
	{
		DestInfo->CopyFrom(InputPlayerInfo);
		DestMoveInfo = DestInfo->mutable_move_info();
	}*/

	SetMoveType(DestMoveInfo->move_type());
}

void ACapstonePlayer::SetPlayerMoveInfo()
{
	FVector Location = GetActorLocation();

	PlayerMoveInfo->set_x(Location.X);
	PlayerMoveInfo->set_y(Location.Y);
	PlayerMoveInfo->set_z(Location.Z);
	PlayerMoveInfo->set_yaw(GetControlRotation().Yaw);
}

void ACapstonePlayer::SetMoveType(Protocol::MoveType MoveType)
{
	PlayerMoveInfo->set_move_type(MoveType);
}

bool ACapstonePlayer::IsSame()
{
	FVector DestLocation = FVector(DestMoveInfo->x(), DestMoveInfo->y(), DestMoveInfo->z());
	FVector PlayerLocation = FVector(PlayerMoveInfo->x(), PlayerMoveInfo->y(), PlayerMoveInfo->z());

	return (DestLocation == PlayerLocation);
}

void ACapstonePlayer::MoveIdle(float DeltaTime)
{
	if (IsSame() == false)
	{
		Move(DeltaTime);
	}

	SetActorRotation(FRotator(0, DestMoveInfo->yaw(), 0));
}

void ACapstonePlayer::MoveRun(float DeltaTime)
{
	// 위치 동기화
	{
		Move(DeltaTime);
	}

	// 바라보는 방향 동기화
	{
		//FRotator ControlRotator = GetControlRotation();
		//FRotator DestYaw = FRotator(0.f, DestMoveInfo->yaw(), 0.f);
		
		//FRotator NextShowRotator = (ControlYaw - DestYaw) * DeltaTime * 500.f;
		//float FinalRotation = FMath::Min(ControlYaw, ControlYaw + NextShowRotator);

		SetActorRotation(FRotator(0.f, DestMoveInfo->yaw(), 0.f));
	}
	
}

void ACapstonePlayer::Move(float DeltaTime)
{
	// 동기화에서 살짝 문제 발생
	// 애니메이션만 틀어주면 완벽함
	FVector Location = GetActorLocation();
	FVector DestLocation = FVector(DestMoveInfo->x(), DestMoveInfo->y(), DestMoveInfo->z());

	FVector MoveDir = DestLocation - Location;

	const float DistToDest = MoveDir.Length();
	MoveDir.Normalize();

	float MoveDist = (MoveDir * DeltaTime * 600.f).Length();
	MoveDist = FMath::Min(MoveDist, DistToDest);
	FVector NextLocation = Location + MoveDist * MoveDir;
	
	SetActorLocation(NextLocation);
	
	//UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	
	//AddMovementInput(MoveDir, 1.f);
}

void ACapstonePlayer::OnSpotLight(bool IsRightOn)
{
	UE_LOG(LogTemp, Warning, TEXT("OnSpotLight"));

	SpotLightComponent->SetVisibility(IsRightOn);
}
