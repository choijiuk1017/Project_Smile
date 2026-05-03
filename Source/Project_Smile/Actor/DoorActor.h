// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"



#include "DoorActor.generated.h"

UCLASS()
class PROJECT_SMILE_API ADoorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorActor();

	UPROPERTY(EditAnywhere)
	UCurveFloat* DoorTimelineFloatCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	FString InteractionText;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* DoorFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Door;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTimelineComponent* DoorTimelineComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* DoorProxVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door")
	FName RequiredItemID = "KeyCard";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door")
	bool bConsumeItemOnOpen = true;

	UPROPERTY(BlueprintReadOnly, Category = "Door")
	bool bIsPlayerInRange = false;

	UPROPERTY(BlueprintReadOnly, Category = "Door")
	bool bIsOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	FString FailText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	FString SuccessText;

	UPROPERTY(EditAnywhere, Category = "Door")
	UStaticMeshComponent* DoorLamp;

	UPROPERTY(EditAnywhere, Category = "Door")
	UMaterialInterface* LockedMaterial;

	UPROPERTY(EditAnywhere, Category = "Door")
	UMaterialInterface* UnlockedMaterial;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Interact(AActor* Interactor);


private:
	FOnTimelineFloat UpdateFunctionFloat;

	UFUNCTION()
	void UpdateTimelineComp(float Output);

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



};
