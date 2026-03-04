// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project_SmileGameMode.h"
#include "Project_SmileCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProject_SmileGameMode::AProject_SmileGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
