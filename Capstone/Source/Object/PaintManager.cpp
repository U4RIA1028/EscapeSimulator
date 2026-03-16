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

	CorrectSequence = { 1, 2, 3 , 4, 5, 6 }; //6개 정도로 늘릴 예정임

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APaintPuzzle::StaticClass(), FoundPuzzles);

	for (AActor* Actor : FoundPuzzles)
	{
		APaintPuzzle* PaintPuzzle = Cast<APaintPuzzle>(Actor);
		if (PaintPuzzle)
		{
			PaintPuzzle->SetManager(this);
		}
	}

	// Find the DeskWithDrawers actor with the specified ID in the world
	TArray<AActor*> FoundDesks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADeskWithDrawers::StaticClass(), FoundDesks);

	for (AActor* Actor : FoundDesks)
	{
		ADeskWithDrawers* Desk = Cast<ADeskWithDrawers>(Actor);
		if (Desk && Desk->DeskID == TargetDeskID)
		{
			DeskWithDrawers = Desk;
			break;
		}
	}
}

// Called every frame
void APaintManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APaintManager::PlayClickSound()
{
	USoundManager::GetInstance()->PlaySoundAtLocation(this, TEXT("/Script/Engine.SoundWave'/Game/Sound/ClickSound.ClickSound'"), GetActorLocation());
}

void APaintManager::PlaySuccessSound()
{
	USoundManager::GetInstance()->PlaySoundAtLocation(this, TEXT("/Script/Engine.SoundWave'/Game/Sound/PuzzleSuccess.PuzzleSuccess'"), GetActorLocation());
}

void APaintManager::PlayFailSound()
{
	USoundManager::GetInstance()->PlaySoundAtLocation(this, TEXT("/Script/Engine.SoundWave'/Game/Sound/FailSound.FailSound'"), GetActorLocation());
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

	if (DeskWithDrawers)
	{
		// Unlock and move the drawer when the puzzle is solved
		if (DrawerToUnlock >= 0 && DrawerToUnlock < DeskWithDrawers->Drawers.Num())
		{
			DeskWithDrawers->Drawers[DrawerToUnlock]->Unlock();
			DeskWithDrawers->Drawers[DrawerToUnlock]->MoveDrawer(true); // 서랍을 움직이게 함
		}
	}
}

void APaintManager::OnPuzzleFailed()
{
	UE_LOG(LogTemp, Warning, TEXT("PuzzleFailed"));

	PlayFailSound();
	PlayerSequence.Empty();
}

void APaintManager::SendPacket()
{
	UCapstoneGameInstance* gameInstnace = Cast<UCapstoneGameInstance>(GWorld->GetGameInstance());
	uint64 PlayerId = gameInstnace->MyPlayer->GetPlayerInfo()->object_id();

	std::string Name = StringUtils::GetString(GetName());

	Protocol::C_ACTION_LOCK_FREE SendPkt;

	SendPkt.set_player_id(PlayerId);
	SendPkt.set_object_name(Name);
	SendPkt.set_is_success(true);
	SendPkt.set_type(Protocol::LockType::LOCK_PUZZLE);

	SEND_PACKET(SendPkt);
}
