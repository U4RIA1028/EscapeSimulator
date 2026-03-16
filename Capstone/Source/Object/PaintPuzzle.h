// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/ObjectBase.h"
#include "Components/BoxComponent.h"
#include "PaintManager.h"
#include "PaintPuzzle.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_API APaintPuzzle : public AObjectBase
{
	GENERATED_BODY()

private:
	APaintPuzzle();

	class APaintManager* PaintManager;

protected:
	virtual void BeginPlay() override;

	virtual void Init() override;

	virtual void Tick(float DeltaTime);

	void OnConstruction(const FTransform& Transform);

	//void OnClickBox(UPrimitiveComponent* ClickedComp, FKey ButtonClicked);

	virtual bool OnClicked(class ACapstoneMyPlayer* Player) override;


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Paint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* Box;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* PictureMaterial;

	UPROPERTY(EditAnywhere)
	int32 paintIndex;

	void SetManager(class APaintManager* NewManager);

	UPROPERTY(BlueprintReadWrite, Category = "Puzzle")
	bool bIsPuzzleSolved;

	
};
