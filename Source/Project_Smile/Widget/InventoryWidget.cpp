// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/InventoryWidget.h"

#include "Component/InventoryComponent.h"
#include "Data/InventoryTypes.h"
#include "Data/ItemData.h"
#include "Widget/InventoryItem.h"


#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"

#include "Blueprint/WidgetTree.h"
#include "Engine/Texture2D.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SelectedItemData = nullptr;

	CreateSlots();

	if (ItemNameText)
	{
		ItemNameText->SetText(FText::FromString(TEXT("아이템 없음")));
	}

	if (ItemDescriptionText)
	{
		ItemDescriptionText->SetText(FText::FromString(TEXT("아이템을 선택하면 설명이 표시됩니다.")));
	}
}

void UInventoryWidget::SetInventoryComponent(UInventoryComponent* InInventoryComponent)
{
	InventoryComponent = InInventoryComponent;

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::RefreshInventory);
	}

	RefreshInventory();
}

void UInventoryWidget::RefreshInventory()
{
	if (!InventoryComponent)
	{
		return;
	}

	CreateSlots();
	CreateItemWidgets();
}

void UInventoryWidget::CreateSlots()
{
	if (!SlotGrid)
	{
		return;
	}

	SlotGrid->ClearChildren();

	const int32 GridWidth = InventoryComponent ? InventoryComponent->GridWidth : 3;
	const int32 GridHeight = InventoryComponent ? InventoryComponent->GridHeight : 3;

	for (int32 Y = 0; Y < GridHeight; Y++)
	{
		for (int32 X = 0; X < GridWidth; X++)
		{
			UBorder* SlotBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());

			if (!SlotBorder)
			{
				continue;
			}

			SlotBorder->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.05f, 0.75f));

			UUniformGridSlot* GridSlot = SlotGrid->AddChildToUniformGrid(SlotBorder, Y, X);

			if (GridSlot)
			{
				GridSlot->SetHorizontalAlignment(HAlign_Fill);
				GridSlot->SetVerticalAlignment(VAlign_Fill);
			}
		}
	}
}

void UInventoryWidget::CreateItemWidgets()
{
	if (!ItemCanvas || !InventoryComponent || !InventoryItemWidgetClass)
	{
		return;
	}

	ItemCanvas->ClearChildren();

	for (const FInventoryItemEntry& Entry : InventoryComponent->Items)
	{
		if (!Entry.ItemData)
		{
			continue;
		}

		UInventoryItem* ItemWidget = CreateWidget<UInventoryItem>(GetWorld(), InventoryItemWidgetClass);

		if (!ItemWidget)
		{
			continue;
		}

		ItemWidget->SetItemData(Entry.ItemData);
		ItemWidget->OnInventoryItemClicked.AddDynamic(this, &UInventoryWidget::SelectItem);

		UCanvasPanelSlot* CanvasSlot = ItemCanvas->AddChildToCanvas(ItemWidget);

		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(FVector2D(Entry.StartX * SlotSize, Entry.StartY * SlotSize));
			CanvasSlot->SetSize(FVector2D(Entry.Width * SlotSize, Entry.Height * SlotSize));
			CanvasSlot->SetAutoSize(false);
		}
	}
}

void UInventoryWidget::SelectItem(UItemData* ItemData)
{
	SelectedItemData = ItemData;

	if (!SelectedItemData)
	{
		if (ItemNameText)
		{
			ItemNameText->SetText(FText::FromString(TEXT("아이템 없음")));
		}

		if (ItemDescriptionText)
		{
			ItemDescriptionText->SetText(FText::FromString(TEXT("아이템을 선택하면 설명이 표시됩니다.")));
		}

		return;
	}

	if (ItemNameText)
	{
		ItemNameText->SetText(SelectedItemData->ItemName);
	}

	if (ItemDescriptionText)
	{
		ItemDescriptionText->SetText(SelectedItemData->ItemDescription);
	}
}