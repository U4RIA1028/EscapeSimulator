// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/PaintPuzzle.h"

APaintPuzzle::APaintPuzzle()
{
	PrimaryActorTick.bCanEverTick = true;

	Paint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PaintMesh"));
	RootComponent = Paint;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("ClickableBox"));
	Box->SetupAttachment(RootComponent);
	Box->SetBoxExtent(FVector(10.f, 10.f, 10.f)); // 콜리전 박스의 크기 설정
	Box->SetRelativeLocation(FVector(0.f, 0.f, -75.f)); // 콜리전 박스의 위치 설정
	Paint->SetCollisionProfileName("IgnoreOnlyPawn");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PaintMeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/HorrorGamePuzzlePack/Models/PaintingPuzzle/Mesh/PaintingPuzzle_SM.PaintingPuzzle_SM'"));
	if (PaintMeshAsset.Succeeded())
	{
		Paint->SetStaticMesh(PaintMeshAsset.Object);
	}

	bIsPuzzleSolved = false;

}

void APaintPuzzle::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (Paint && PictureMaterial)
	{
		Paint->SetMaterial(3, PictureMaterial);
	}
}

void APaintPuzzle::BeginPlay()
{
	Super::BeginPlay();
}


void APaintPuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APaintPuzzle::Init()
{
	ObjectInfo->set_object_type(Protocol::ObjectType::OBJECT_IDLE);

	Super::Init();
}


bool APaintPuzzle::OnClicked(class ACapstoneMyPlayer* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Interact!"))
	if (bIsPuzzleSolved)
	{
		return true; // 퍼즐이 해결된 상태에서는 더 이상 클릭을 처리하지 않음
	}

	if (PaintManager)
	{
		PaintManager->OnImageClicked(paintIndex);
	}

	return true;
}

void APaintPuzzle::SetManager(APaintManager* NewManager)
{
	PaintManager = NewManager;
}