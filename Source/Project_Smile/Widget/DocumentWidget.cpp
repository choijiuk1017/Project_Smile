// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/DocumentWidget.h"

#include "Components/TextBlock.h"
#include "Data/ItemData.h"
#include "Actor/DocumentActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

#include "Input/Reply.h"
#include "InputCoreTypes.h"

void UDocumentWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
}

FReply UDocumentWidget::NativeOnPreviewKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent
)
{
	if (InKeyEvent.GetKey() == EKeys::SpaceBar)
	{
		OnCloseButtonClicked();
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply UDocumentWidget::NativeOnPreviewMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	SetFocus();
	SetKeyboardFocus();

	return FReply::Handled();
}

void UDocumentWidget::SetDocumentData(UItemData* InDocumentData)
{
	DocumentData = InDocumentData;

	if (!DocumentData)
	{
		return;
	}

	if (DocumentTitleText)
	{
		DocumentTitleText->SetText(DocumentData->ItemName);
	}

	if (DocumentContentText)
	{
		DocumentContentText->SetText(DocumentData->ItemDescription);
	}
}

void UDocumentWidget::SetOwningDocumentActor(ADocumentActor* InDocumentActor)
{
	OwningDocumentActor = InDocumentActor;
}

void UDocumentWidget::OnCloseButtonClicked()
{
	if (OwningDocumentActor)
	{
		OwningDocumentActor->CloseDocument();
		return;
	}

	RemoveFromParent();
}