// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"

#include "InventoryItem.generated.h"

class UImage;
class UTextBlock;
class UItemData;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemClicked, UItemData*, ItemData);

UCLASS()
class PROJECT_SMILE_API UInventoryItem : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory Item")
	void SetItemData(UItemData* InItemData);

	UFUNCTION(BlueprintCallable, Category = "Inventory Item")
	UItemData* GetItemData() const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory Item")
	FOnInventoryItemClicked OnInventoryItemClicked;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* CountText;

private:
	UPROPERTY()
	UItemData* ItemData;
};
