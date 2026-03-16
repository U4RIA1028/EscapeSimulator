// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/ObjectBase.h"
#include "SoundManager.h"
#include "NoteObject.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_API ANoteObject : public AObjectBase
{
	GENERATED_BODY()


public:
	ANoteObject();

protected:
	
	virtual void BeginPlay() override;

	virtual void Init();

	virtual bool OnClicked(class ACapstoneMyPlayer* Player) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Note;
	
	UPROPERTY(EditInstanceOnly, Category = Note)
	TSubclassOf<class UNoteBaseWidget> NoteWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note", meta = (AllowPrivateAccess = true))
		UMaterialInterface* NoteMaterial;

	UUserWidget* NoteWidgetInstance;

	bool bNoteWidgetVisible;
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetNoteWidgetClass(TSubclassOf<UUserWidget> NewNoteWidgetClass);

	void SetMaterial(UMaterialInterface* Material);

	void PlaySound();
	


};
