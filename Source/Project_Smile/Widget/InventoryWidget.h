#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UInventoryComponent;
class UUniformGridPanel;
class UCanvasPanel;
class UTextBlock;
class UInventoryItem;
class UInventorySlotWidget;
class UItemData;
class UItemInspectWidget;


UCLASS()
class PROJECT_SMILE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetInventoryComponent(UInventoryComponent* InInventory);

	UFUNCTION()
	void RefreshInventory();

	UFUNCTION()
	void OnSlotClicked(int32 X, int32 Y);

	UFUNCTION()
	void SelectItem(UItemData* ItemData);

protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* SlotGrid;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* ItemCanvas;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescriptionText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UInventoryItem> InventoryItemWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UInventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SlotSize = 130.0f;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UItemInspectWidget> InspectWidgetClass;

	UPROPERTY()
	UItemInspectWidget* InspectWidget;

private:
	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	UPROPERTY()
	UItemData* SelectedItem;

	void OpenInspectWindow();
};