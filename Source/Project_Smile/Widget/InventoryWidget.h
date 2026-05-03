// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UUniformGridPanel;
class UCanvasPanel;
class UTextBlock;
class UBorder;
class UImage;

class UItemData;
class UInventoryComponent;
class UInventoryItem;
/**
 * 
 */
UCLASS()
class PROJECT_SMILE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetInventoryComponent(UInventoryComponent* InInventoryComponent);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefreshInventory();

	UFUNCTION()
	void SelectItem(UItemData* ItemData);

protected:
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* SlotGrid;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* ItemCanvas;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescriptionText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory UI")
	TSubclassOf<UInventoryItem> InventoryItemWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory UI")
	float SlotSize = 80.0f;

private:
	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	UPROPERTY()
	UItemData* SelectedItemData;

private:
	void CreateSlots();
	void CreateItemWidgets();
};
