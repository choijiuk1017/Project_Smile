// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"

#include "InventorySlotWidget.generated.h"

class UImage;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotClicked, int32, GridX, int32, GridY);

UCLASS()
class PROJECT_SMILE_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
	void InitSlot(int32 InGridX, int32 InGridY);

	UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
	void SetSelected(bool bSelected);

	UPROPERTY(BlueprintAssignable, Category = "Inventory Slot")
	FOnInventorySlotClicked OnInventorySlotClicked;

	UFUNCTION(BlueprintCallable)
	void SetOccupied(bool bOccupied);

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* SlotImage;

private:
	UPROPERTY()
	int32 GridX = 0;

	UPROPERTY()
	int32 GridY = 0;

	UPROPERTY()
	bool bIsSelected = false;

	
};
