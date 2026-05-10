// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

UCLASS()
class PROJECT_SMILE_API AInteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActor();

	// 상호작용 텍스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	FString InteractionText;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName RequiredItemID = "None";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	bool bConsumeItemOnInteract = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	FString FailText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	FString SuccessText;

	UPROPERTY(BlueprintReadOnly, Category = "Door")
	bool bIsPlayerInRange = false;

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void Interact(AActor* Interactor) {};
};
