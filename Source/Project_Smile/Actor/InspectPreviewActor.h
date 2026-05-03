// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InspectPreviewActor.generated.h"

class UStaticMeshComponent;


UCLASS()
class PROJECT_SMILE_API AInspectPreviewActor : public AActor
{
	GENERATED_BODY()
	
public:
	AInspectPreviewActor();

public:
	UFUNCTION(BlueprintCallable)
	void SetPreviewMesh(UStaticMesh* InMesh);

	UFUNCTION(BlueprintCallable)
	void AddPreviewRotation(float DeltaYaw, float DeltaPitch);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PreviewMeshComponent;

};
