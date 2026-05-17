// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TitlePlayerController.generated.h"

class UTitleWidget;

/**
 * 
 */
UCLASS()
class PROJECT_SMILE_API ATitlePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTitleWidget> TitleWidgetClass;

private:
	UPROPERTY()
	UTitleWidget* TitleWidget;
	
};
