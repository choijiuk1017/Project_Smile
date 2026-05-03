// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/InventoryItem.h"

#include "Data/ItemData.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UInventoryItem::NativeConstruct()
{
	Super::NativeConstruct();

	if (CountText)
	{
		CountText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryItem::SetItemData(UItemData* InItemData)
{
	ItemData = InItemData;

	if (!ItemData)
	{
		return;
	}

	if (ItemImage && ItemData->ItemIcon)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(ItemData->ItemIcon);
		ItemImage->SetBrush(Brush);
	}

	SetToolTipText(ItemData->ItemName);
}

UItemData* UInventoryItem::GetItemData() const
{
	return ItemData;
}

FReply UInventoryItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnInventoryItemClicked.Broadcast(ItemData);
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}