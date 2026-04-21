// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Smile/Widget/CaptureSelection.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

bool UCaptureSelection::Initialize()
{
	const bool bResult = Super::Initialize();

	SetIsFocusable(true);

	return bResult;
}

void UCaptureSelection::NativeConstruct()
{
	Super::NativeConstruct();

	SetKeyboardFocus();

	ResetSelection();
	UpdateSelectionVisual();
}

FReply UCaptureSelection::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey PressedKey = InKeyEvent.GetKey();

	if (PressedKey == EKeys::Up)
	{
		AdjustVerticalSize(+ResizeStep);
		return FReply::Handled();
	}
	else if (PressedKey == EKeys::Down)
	{
		AdjustVerticalSize(-ResizeStep);
		return FReply::Handled();
	}
	else if (PressedKey == EKeys::Left)
	{
		AdjustHorizontalSize(+ResizeStep);
		return FReply::Handled();
	}
	else if (PressedKey == EKeys::Right)
	{
		AdjustHorizontalSize(-ResizeStep);
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UCaptureSelection::AdjustVerticalSize(float Delta)
{
	SelectionSize.Y += Delta;
	ClampSelectionSize();
	UpdateSelectionVisual();
}

void UCaptureSelection::AdjustHorizontalSize(float Delta)
{
	SelectionSize.X += Delta;
	ClampSelectionSize();
	UpdateSelectionVisual();
}

void UCaptureSelection::GetSelectionRect(FVector2D& OutPosition, FVector2D& OutSize) const
{
	OutPosition = SelectionPosition;
	OutSize = SelectionSize;
}

void UCaptureSelection::ResetSelection()
{
	SelectionPosition = DefaultSelectionPosition;
	SelectionSize = DefaultSelectionSize;

	ClampSelectionSize();
	UpdateSelectionVisual();
}

void UCaptureSelection::ClampSelectionSize()
{
	SelectionSize.X = FMath::Clamp(SelectionSize.X, MinSelectionSize.X, MaxSelectionSize.X);
	SelectionSize.Y = FMath::Clamp(SelectionSize.Y, MinSelectionSize.Y, MaxSelectionSize.Y);
}

void UCaptureSelection::UpdateSelectionVisual()
{
	if (!SelectionBorder)
	{
		return;
	}

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(SelectionBorder->Slot))
	{
		CanvasSlot->SetPosition(SelectionPosition);
		CanvasSlot->SetSize(SelectionSize);
	}
}