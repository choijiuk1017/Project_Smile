// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/MainPlayerController.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);

	bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}