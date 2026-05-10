// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/InteractableActor.h"
#include "LightSwitchActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_SMILE_API ALightSwitchActor : public AInteractableActor
{
	GENERATED_BODY()
public:
	ALightSwitchActor();

	void TurnOnSwitch();

	virtual void Interact(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Switch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* SwitchProxVolume;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Light")
	TArray<AActor*> TargetLampActors;

private:

	bool bIsOperated;
};
