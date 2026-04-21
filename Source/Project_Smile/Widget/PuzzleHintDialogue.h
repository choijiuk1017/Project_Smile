// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PuzzleHintDialogue.generated.h"


UCLASS()
class PROJECT_SMILE_API UPuzzleHintDialogue : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget = "true"))
	TObjectPtr<class UTextBlock> TXT_Dialogue;

	void SetDialogue(FString Dialogue);
};
