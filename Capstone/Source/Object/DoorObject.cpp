// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Door/DoorObject.h"
#include "CapstoneMyPlayer.h"
#include "../../Utils/StringUtils.h"
#include "Capstone.h"

ADoorObject::ADoorObject()
{
	PrimaryActorTick.bCanEverTick = true;

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));

	Box->SetupAttachment(RootComponent);
	Door->SetupAttachment(Box);

	Door->SetCollisionProfileName(TEXT("BlockAll"));
	Box->SetCollisionProfileName(TEXT("Trigger"));

	bIsClosed = true;
}

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

void ADoorObject::Init()
{
	ObjectInfo->set_object_type(Protocol::ObjectType::OBJECT_DOOR);

	Super::Init();
}

void ADoorObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timeline.TickTimeline(DeltaTime);
}

void ADoorObject::PlaySound()
{
	USoundManager::GetInstance()->PlaySoundAtLocation(this, TEXT("/Script/Engine.SoundWave'/Game/Sound/WoodenDoor.WoodenDoor'"), GetActorLocation());
}

void ADoorObject::OpenDoor(float Value)
{
	FRotator DoorNewRotation = FRotator(0.0f, Value, 0.f);
	Door->SetRelativeRotation(DoorNewRotation);
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

void ADoorObject::SendPacket(const uint64 PlayerId)
{
	FString Name = GetName();

	Protocol::C_ACTION_DOOR doorAction;
	doorAction.set_player_id(PlayerId);
	doorAction.set_object_name(StringUtils::GetString(Name));
	doorAction.set_is_open_door(bIsClosed);

	SEND_PACKET(doorAction);
}