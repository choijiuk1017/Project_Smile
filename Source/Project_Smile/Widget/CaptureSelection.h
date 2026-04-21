// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CaptureSelection.generated.h"

class UCanvasPanel;
class UBorder;

/**
 * 
 */
UCLASS()
class PROJECT_SMILE_API UCaptureSelection : public UUserWidget
{
	GENERATED_BODY()
	

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION(BlueprintCallable)
	void GetSelectionRect(FVector2D& OutPosition, FVector2D& OutSize) const;

	UFUNCTION(BlueprintCallable)
	void ResetSelection();

protected:
	void UpdateSelectionVisual();
	void ClampSelectionSize();

	void AdjustVerticalSize(float Delta);
	void AdjustHorizontalSize(float Delta);

protected:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* RootCanvas;

	UPROPERTY(meta = (BindWidget))
	UBorder* SelectionBorder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	FVector2D SelectionPosition = FVector2D(-200.f, -200.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	FVector2D SelectionSize = FVector2D(400.f, 400.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	FVector2D MinSelectionSize = FVector2D(100.f, 100.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	FVector2D MaxSelectionSize = FVector2D(1500.f, 1000.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	FVector2D DefaultSelectionPosition = FVector2D(-200.f, -200.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	FVector2D DefaultSelectionSize = FVector2D(400.f, 400.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	float ResizeStep = 20.f;
};
