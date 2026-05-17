// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/TitlePlayerController.h"

#include "Widget/TitleWidget.h"
#include "Blueprint/UserWidget.h"

void ATitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	bShowMouseCursor = false;

	if (TitleWidgetClass)
	{
		TitleWidget = CreateWidget<UTitleWidget>(this, TitleWidgetClass);

		if (TitleWidget)
		{
			TitleWidget->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(TitleWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			SetInputMode(InputMode);

			bShowMouseCursor = false;

			TitleWidget->SetKeyboardFocus();
		}
	}
}