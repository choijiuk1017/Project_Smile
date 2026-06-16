// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/AnalysisLoadingWidget.h"

#include "Components/TextBlock.h"
#include "TimerManager.h"

void UAnalysisLoadingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DotCount = 0;
	UpdateLoadingText();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			DotTimerHandle,
			this,
			&UAnalysisLoadingWidget::UpdateLoadingText,
			0.4f,
			true
		);
	}
}

void UAnalysisLoadingWidget::NativeDestruct()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DotTimerHandle);
	}

	Super::NativeDestruct();
}

void UAnalysisLoadingWidget::UpdateLoadingText()
{
	if (!LoadingText)
	{
		return;
	}

	DotCount++;

	if (DotCount > 3)
	{
		DotCount = 0;
	}

	FString Dots;

	for (int32 i = 0; i < DotCount; i++)
	{
		Dots += TEXT(".");
	}

	LoadingText->SetText(FText::FromString(TEXT("ANALYZING") + Dots));
}