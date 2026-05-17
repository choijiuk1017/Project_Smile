// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class PROJECT_SMILE_API UTitleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

	virtual FReply NativeOnPreviewMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* StartText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ExitText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Menu")
	FName GameLevelName = TEXT("MainLevel");

private:
	int32 SelectedIndex = 0;

	void MoveSelection(int32 Direction);
	void ExecuteSelection();
	void UpdateSelectionVisual();
};
