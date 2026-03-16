// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/PaintManager.h"
#include "PaintPuzzle.h"
#include "Kismet/GameplayStatics.h"
#include "CapstoneGameInstance.h"
#include "Capstone.h"
#include "CapstoneMyPlayer.h"
#include "../Utils/StringUtils.h"

// Sets default values
APaintManager::APaintManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TargetDeskID = 11;
	DrawerToUnlock = 3;
}

// Called when the game starts or when spawned
void APaintManager::BeginPlay()
{
	Super::BeginPlay();

	CorrectSequence = { 1, 2, 3 , 4, 5, 6 };

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APaintPuzzle::StaticClass(), FoundPuzzles);

	for (AActor* Actor : FoundPuzzles)
	{
		APaintPuzzle* PaintPuzzle = Cast<APaintPuzzle>(Actor);
		if (PaintPuzzle)
		{
			PaintPuzzle->SetManager(this);
		}
	}
}

void APaintManager::OnImageClicked(int32 ClickedIndex)
{

	PlayerSequence.Add(ClickedIndex);
	UE_LOG(LogTemp, Warning, TEXT("Paint index : %d"), ClickedIndex);

	int32 CurrentIndex = PlayerSequence.Num() - 1;
	if (PlayerSequence[CurrentIndex] == CorrectSequence[CurrentIndex])
	{
		PlayClickSound();
		if (PlayerSequence.Num() == CorrectSequence.Num())
		{
			SendPacket();

			OnPuzzleSolved();
		}
	}
	else
	{
		OnPuzzleFailed();
	}
}

void APaintManager::OnPuzzleSolved()
{
	UE_LOG(LogTemp, Warning, TEXT("PuzzleSolved"));

	for (AActor* Actor : FoundPuzzles)
	{
		APaintPuzzle* PaintPuzzle = Cast<APaintPuzzle>(Actor);
		if (PaintPuzzle)
		{
			PaintPuzzle->bIsPuzzleSolved = true;
		}
	}
}

void APaintManager::OnPuzzleFailed()
{
	UE_LOG(LogTemp, Warning, TEXT("PuzzleFailed"));

	PlayFailSound();
	PlayerSequence.Empty();
}
