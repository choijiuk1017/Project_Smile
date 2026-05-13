// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/InteractableActor.h"
#include "DocumentActor.generated.h"


class UStaticMeshComponent;
class UBoxComponent;
class UItemData;
class UDocumentWidget;
class UCameraComponent;
/**
 * 
 */
UCLASS()
class PROJECT_SMILE_API ADocumentActor : public AInteractableActor
{
	GENERATED_BODY()

public:
	ADocumentActor();

	virtual void Interact(AActor* Interactor) override;

	UFUNCTION(BlueprintCallable)
	void CloseDocument();


protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Document")
	UStaticMeshComponent* DocumentMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Document")
	UBoxComponent* InteractionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Document")
	UItemData* DocumentData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Document")
	TSubclassOf<UDocumentWidget> DocumentWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Document")
	UCameraComponent* DocumentCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Document")
	float CameraBlendTime = 0.3f;

private:
	UPROPERTY()
	UDocumentWidget* CurrentDocumentWidget;

	UPROPERTY()
	AActor* PreviousViewTarget;

	UPROPERTY()
	APlayerController* CachedPlayerController;

	virtual void CalcCamera(
		float DeltaTime,
		struct FMinimalViewInfo& OutResult
	) override;
};
