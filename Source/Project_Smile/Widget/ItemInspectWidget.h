// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemInspectWidget.generated.h"



class UItemData;
class UTextBlock;
class UImage;
class AInspectPreviewActor;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
/**
 * 
 */
UCLASS()
class PROJECT_SMILE_API UItemInspectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
	UFUNCTION(BlueprintCallable)
	void SetItemData(UItemData* InItemData);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescriptionText;

	UPROPERTY(meta = (BindWidget))
	UImage* PreviewImage;

private:
	UPROPERTY()
	UItemData* ItemData;

	UPROPERTY()
	AInspectPreviewActor* PreviewActor;

	UPROPERTY()
	AActor* CaptureActor;

	UPROPERTY()
	USceneCaptureComponent2D* SceneCaptureComponent;

	UPROPERTY()
	UTextureRenderTarget2D* RenderTarget;

	bool bDragging = false;
	FVector2D LastMousePosition;

private:
	void CreatePreviewScene();
	void DestroyPreviewScene();
	void ApplyRenderTargetToImage();
	void CloseInspectWidget();
};
