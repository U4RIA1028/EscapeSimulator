// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/PaintPuzzle.h"

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
