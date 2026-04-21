// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Smile/Widget/PuzzleHintDialogue.h"
#include "Components/TextBlock.h"

void UPuzzleHintDialogue::SetDialogue(FString Dialogue)
{
	TXT_Dialogue->SetText(FText::FromString(Dialogue));
}
