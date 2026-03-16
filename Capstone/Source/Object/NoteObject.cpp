// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/NoteObject.h"
#include "NoteBaseWidget.h"
#include "Components/WidgetComponent.h"

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
