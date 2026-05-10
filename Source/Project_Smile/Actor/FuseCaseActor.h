// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/InteractableActor.h"
#include "Components/TimelineComponent.h"

#include "FuseCaseActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_SMILE_API AFuseCaseActor : public AInteractableActor
{
	GENERATED_BODY()

public:
	AFuseCaseActor();

	UPROPERTY(EditAnywhere)
	UCurveFloat* CaseTimelineFloatCurve;

	virtual void Tick(float DeltaTime) override;
	virtual void Interact(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CaseFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CaseDoor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTimelineComponent* CaseTimelineComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* CaseProxVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fuse")
	int MaxFuseNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fuse")
	int NeedFuseNum;

	UPROPERTY(BlueprintReadOnly, Category = "FuseDoor")
	bool bIsOpen = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fuse")
	TArray<UStaticMeshComponent*> FuseMeshes;


	UPROPERTY(BlueprintReadOnly, Category = "Fuse")
	TArray<int32> EmptyFuseIndices;

private:
	FOnTimelineFloat UpdateFunctionFloat;

	UFUNCTION()
	void UpdateTimelineComp(float Output);

	void RandomizeMissingFuses();


};
