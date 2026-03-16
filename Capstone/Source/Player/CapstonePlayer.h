// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Protocol.pb.h"
#include "CapstonePlayer.generated.h"

UCLASS()
class CAPSTONE_API ACapstonePlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACapstonePlayer();
	virtual ~ACapstonePlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UCameraComponent> CameraComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	Protocol::ObjectInfo* GetPlayerInfo() { return PlayerInfo; }
	Protocol::ObjectInfo* GetDestInfo() { return DestInfo; }

	void SetPlayerInfo(const Protocol::ObjectInfo& InputPlayerInfo);
	void SetDestInfo(const Protocol::ObjectInfo& InputPlayerInfo);

	bool GetIsMine() { return IsMine; }
	void SetIsMine(bool IsPlayer) { IsMine = IsPlayer; }

	void SetPlayerMoveInfo();
	void SetMoveType(Protocol::MoveType MoveType);

	bool IsSame();

private:
	void MoveIdle(float DeltaTime);
	void MoveRun(float DeltaTime);
	
	void Move(float DeltaTime);

public:
	void OnSpotLight(bool IsRightOn);

private:
	FVector StopLoaction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Light, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpotLightComponent> SpotLightComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Light, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPointLightComponent> PointLightComponent;

protected:
	class Protocol::ObjectInfo* PlayerInfo; // 현재 위치
	class Protocol::MoveInfo* PlayerMoveInfo;

	class Protocol::ObjectInfo* DestInfo;
	class Protocol::MoveInfo* DestMoveInfo;
	
public:
	bool IsMine;
};
