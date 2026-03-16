// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeskWithDrawers.h"
#include "SoundManager.h"
#include "PaintManager.generated.h"

UCLASS()
class CAPSTONE_API APaintManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APaintManager();

	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void OnImageClicked(int32 paintindex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	TArray<int32> CorrectSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	int32 TargetDeskID; // 상호작용할 Desk의 ID

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	int32 DrawerToUnlock; // 잠금 해제할 서랍의 인덱스

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PlayClickSound();

	void PlayFailSound();

	void PlaySuccessSound();

	void OnPuzzleSolved();

private:

	TArray<int32> PlayerSequence;
	int32 TargetIndex;

	void OnPuzzleFailed();

	void SendPacket();

	TArray<AActor*> FoundPuzzles;

	ADeskWithDrawers* DeskWithDrawers; // 찾은 DeskWithDrawers 인스턴스
};
