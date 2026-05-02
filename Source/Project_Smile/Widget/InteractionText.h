// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionText.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_SMILE_API UInteractionText : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget = "true"))
	TObjectPtr<class UTextBlock> TXT_Text;

	void SetText(FString Text);
};
