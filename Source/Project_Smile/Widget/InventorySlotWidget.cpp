// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/InventorySlotWidget.h"

#include "Components/Image.h"
#include "Input/Reply.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetSelected(false);
}

void UInventorySlotWidget::InitSlot(int32 InGridX, int32 InGridY)
{
	GridX = InGridX;
	GridY = InGridY;
}

void UInventorySlotWidget::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;

	if (!SlotImage)
	{
		return;
	}

	if (bIsSelected)
	{
		SlotImage->SetColorAndOpacity(FLinearColor(0.8f, 0.8f, 1.0f, 1.0f));
	}
	else
	{
		SlotImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.75f));
	}
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnInventorySlotClicked.Broadcast(GridX, GridY);
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (!bIsSelected && SlotImage)
	{
		SlotImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (!bIsSelected && SlotImage)
	{
		SlotImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.75f));
	}
}

void UInventorySlotWidget::SetOccupied(bool bOccupied)
{
	if (!SlotImage)
	{
		return;
	}

	if (bOccupied)
	{
		SlotImage->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SlotImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.75f));
	}
}