// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DocumentWidget.generated.h"

class UTextBlock;
class UItemData;
class ADocumentActor;


/**
 * 
 */
UCLASS()
class PROJECT_SMILE_API UDocumentWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetDocumentData(UItemData* InDocumentData);
	void SetOwningDocumentActor(ADocumentActor* InDocumentActor);

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnPreviewKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
	) override;

	virtual FReply NativeOnPreviewMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DocumentTitleText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DocumentContentText;


private:
	UFUNCTION()
	void OnCloseButtonClicked();

private:
	UPROPERTY()
	UItemData* DocumentData;

	UPROPERTY()
	ADocumentActor* OwningDocumentActor;
};
