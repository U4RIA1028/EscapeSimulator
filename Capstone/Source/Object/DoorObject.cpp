// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Door/DoorObject.h"
#include "CapstoneMyPlayer.h"
#include "../../Utils/StringUtils.h"
#include "Capstone.h"

void ADoorObject::BeginPlay()
{
	Super::BeginPlay();

	if (DoorTimelineFloatCurve)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &ADoorObject::OpenDoor);
		Timeline.AddInterpFloat(DoorTimelineFloatCurve, TimelineProgress);
	}
}

void ADoorObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timeline.TickTimeline(DeltaTime);
}

bool ADoorObject::OnClicked(class ACapstoneMyPlayer* Player)
{
	OpenClosedDoor(bIsClosed);
	PlaySound();
	SendPacket(Player->GetPlayerInfo()->object_id());

	return true;
}

void ADoorObject::OpenClosedDoor(bool IsClosed)
{
	if (IsClosed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Open!"));
		Timeline.Play();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Close!"));
		Timeline.Reverse();
	}

	bIsClosed = !bIsClosed;
}
