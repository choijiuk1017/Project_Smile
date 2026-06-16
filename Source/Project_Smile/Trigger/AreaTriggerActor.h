// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AreaTriggerActor.generated.h"

class UBoxComponent;

UCLASS()
class PROJECT_SMILE_API AAreaTriggerActor : public AActor
{
	GENERATED_BODY()
	
public:
	AAreaTriggerActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Area")
	UBoxComponent* AreaTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
	FString AreaID;

	UFUNCTION()
	void OnAreaBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
