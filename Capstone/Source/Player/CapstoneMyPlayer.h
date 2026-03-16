// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/CapstonePlayer.h"
#include "Key/ItemKey.h"
#include "Object/Key/ItemKey.h"
#include "PickupObject.h"
#include "CapstoneMyPlayer.generated.h"

struct FInputActionValue;
class UInputAction;
class AObjectBase;
/**
 *
 */
UCLASS()
class CAPSTONE_API ACapstoneMyPlayer : public ACapstonePlayer
{
	GENERATED_BODY()

public:
	ACapstoneMyPlayer();
	//virtual ~ACapstoneMyPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetKeyCount(int num);
	int GetKeyCount();

	void RoomCount();
	void ResetRoomCount();
	int GetRoomCount();
	int inventory[30];
	void SetInventory(int id);
	int GetInventory(int i);
	void InventoryClear();

public:
	bool IsFindActor = false;

protected:

private:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TurnRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> OpenMenuAction;

	//<Pickup>
	UPROPERTY(EditAnywhere)
	class USceneComponent* HoldingComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	UPROPERTY(EditAnywhere)
	class AItemKey* Key;

	UPROPERTY(EditAnywhere)
	class APickupObject* Object;


	int KeyCount;
	int ClearRoomCount;

	bool bCanMove;
	bool bHoldingItem;
	bool bInspecting;

	float PitchMax;
	float PitchMin;

	FVector HoldingComp;
	FRotator LastRotation;

	FVector StartDebug;
	FVector ForwardVector;
	FVector EndDebug;
	FHitResult Hit;

	FComponentQueryParams DefaultComponentQueryParams;
	FCollisionResponseParams DefaultResponseParams;

	void ToggleItemPickup();
	//</Pickup>

protected:
	/** Called for movement input */
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Stop(const FInputActionValue& Value);

	/** Called for looking input */
	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void Clicked(const FInputActionValue& Value);

	UFUNCTION()
	void TurnRight(const FInputActionValue& Value);

	UFUNCTION()
	void OpenMenu(const FInputActionValue& Value);

	void SendSyncMove();

	void ToggleMenuPanel();


	// Clicked ÇÔ¼ö¿ë
private:
	void ObjectBaseClicked(class AObjectBase* Base);
	AObjectBase* FindActorBase(FVector Start, FVector End);

	std::pair<FVector, FVector> CameraVector();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

public:
	TObjectPtr<class UPlayerBaseWidget> PlayerWidget;

private:
	const float MOVE_PACKET_SEND_DELAY = 0.1f;

	UPROPERTY()
	float PacketSendCountTime = MOVE_PACKET_SEND_DELAY;

	bool IsRightOn = false;
};
