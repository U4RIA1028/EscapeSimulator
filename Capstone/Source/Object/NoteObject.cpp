// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/NoteObject.h"
#include "NoteBaseWidget.h"
#include "Components/WidgetComponent.h"

ANoteObject::ANoteObject()
{
	Note = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NoteMesh"));
	Note->SetupAttachment(RootComponent);
	Note->SetCollisionProfileName(TEXT("BlockAll"));
	Note->SetMobility(EComponentMobility::Movable);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> NoteMeshAsset(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (NoteMeshAsset.Succeeded())
	{
		Note->SetStaticMesh(NoteMeshAsset.Object);
	}

	NoteWidgetClass = UNoteBaseWidget::StaticClass();

	bNoteWidgetVisible = false;
}

void ANoteObject::BeginPlay()
{
	Super::BeginPlay();
}

void ANoteObject::Init()
{
	ObjectInfo->set_object_type(Protocol::ObjectType::OBJECT_IDLE);

	Super::Init();
}

void ANoteObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANoteObject::PlaySound()
{
	USoundManager::GetInstance()->PlaySoundAtLocation(this, TEXT("/Script/Engine.SoundWave'/Game/Sound/NoteSound.NoteSound'"), GetActorLocation());
}

void ANoteObject::SetMaterial(UMaterialInterface* Material)
{
	UStaticMeshComponent* MeshComponent = FindComponentByClass<UStaticMeshComponent>();
	if (MeshComponent && Material)
	{
		MeshComponent->SetMaterial(0, Material);
	}
}

bool ANoteObject::OnClicked(class ACapstoneMyPlayer* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Interact!"));

	bNoteWidgetVisible = !bNoteWidgetVisible;

	if (bNoteWidgetVisible)
	{
		NoteWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), NoteWidgetClass);

		if (NoteWidgetInstance)
		{
			PlaySound();
			NoteWidgetInstance->AddToViewport();
		}
	}
	else
	{
		if (NoteWidgetInstance && NoteWidgetInstance->IsInViewport())
		{
			NoteWidgetInstance->RemoveFromViewport();
			NoteWidgetInstance = nullptr;
		}
	}

	return true;
}

void ANoteObject::SetNoteWidgetClass(TSubclassOf<UUserWidget> NewNoteWidgetClass)
{
	NoteWidgetClass = NewNoteWidgetClass;
}