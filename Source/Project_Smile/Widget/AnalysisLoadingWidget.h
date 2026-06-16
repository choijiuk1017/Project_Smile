// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnalysisLoadingWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECT_SMILE_API UAnalysisLoadingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LoadingText;

private:
	FTimerHandle DotTimerHandle;

	int32 DotCount = 0;

	void UpdateLoadingText();
};
