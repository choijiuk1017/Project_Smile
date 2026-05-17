// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/TitleWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "Framework/Application/SlateApplication.h"

void UTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	SelectedIndex = 0;
	UpdateSelectionVisual();

	SetKeyboardFocus();

	FSlateApplication::Get().SetKeyboardFocus(TakeWidget(), EFocusCause::SetDirectly);
}

FReply UTitleWidget::NativeOnPreviewKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent
)
{
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Up || Key == EKeys::W)
	{
		MoveSelection(-1);
		return FReply::Handled();
	}

	if (Key == EKeys::Down || Key == EKeys::S)
	{
		MoveSelection(1);
		return FReply::Handled();
	}

	if (Key == EKeys::Enter)
	{
		ExecuteSelection();
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UTitleWidget::MoveSelection(int32 Direction)
{
	SelectedIndex += Direction;

	if (SelectedIndex < 0)
	{
		SelectedIndex = 1;
	}
	else if (SelectedIndex > 1)
	{
		SelectedIndex = 0;
	}

	UpdateSelectionVisual();
}

void UTitleWidget::ExecuteSelection()
{
	if (SelectedIndex == 0)
	{
		UGameplayStatics::OpenLevel(this, "Start");
	}
	else if (SelectedIndex == 1)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

		UKismetSystemLibrary::QuitGame(
			this,
			PC,
			EQuitPreference::Quit,
			false
		);
	}
}

FReply UTitleWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	SetKeyboardFocus();

	return FReply::Handled()
		.SetUserFocus(TakeWidget(), EFocusCause::Mouse);
}

FReply UTitleWidget::NativeOnPreviewMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	SetKeyboardFocus();

	return FReply::Handled()
		.SetUserFocus(TakeWidget(), EFocusCause::Mouse);
}

void UTitleWidget::UpdateSelectionVisual()
{
	if (!StartText || !ExitText)
	{
		return;
	}

	if (SelectedIndex == 0)
	{
		StartText->SetText(FText::FromString(TEXT("> Start")));
		ExitText->SetText(FText::FromString(TEXT("  Exit")));
	}
	else
	{
		StartText->SetText(FText::FromString(TEXT("  Start")));
		ExitText->SetText(FText::FromString(TEXT("> Exit")));
	}
}