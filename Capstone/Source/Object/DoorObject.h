// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/ObjectBase.h"
#include "Components/BoxComponent.h"
#include "SoundManager.h"
#include "Components/TimelineComponent.h"
#include "DoorObject.generated.h"

/**
 *
 */
UCLASS()
class CAPSTONE_API ADoorObject : public AObjectBase
{
	GENERATED_BODY()

public:

	ADoorObject();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OpenDoor(float Value);

	void PlaySound();

protected:
	virtual void BeginPlay() override;

	virtual void Init() override;

	virtual bool OnClicked(class ACapstoneMyPlayer* Player) override;

public:
	void OpenClosedDoor(bool IsClosed);
	void SendPacket(const uint64 PlayerId);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Door;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* Box;

public:

	UPROPERTY(EditAnywhere)
	UCurveFloat* DoorTimelineFloatCurve;

	UPROPERTY(EditAnywhere)
	FTimeline Timeline;

	bool bIsClosed;
};
