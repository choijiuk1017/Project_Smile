#include "Widget/InventoryWidget.h"

#include "Component/InventoryComponent.h"
#include "Widget/InventoryItem.h"
#include "Widget/InventorySlotWidget.h"
#include "Widget/ItemInspectWidget.h"
#include "Data/InventoryTypes.h"
#include "Data/ItemData.h"
#include "Project_SmileCharacter.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "InputCoreTypes.h"
#include "GameFramework/PlayerController.h"


void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();


	if (ItemNameText)
	{
		ItemNameText->SetText(FText::FromString(TEXT("아이템 없음")));
	}

	if (ItemDescriptionText)
	{
		ItemDescriptionText->SetText(FText::FromString(TEXT("설명 없음")));
	}
}


void UInventoryWidget::SetInventoryComponent(UInventoryComponent* InInventory)
{
	InventoryComponent = InInventory;

	if (!InventoryComponent)
	{
		return;
	}

	InventoryComponent->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::RefreshInventory);

	RefreshInventory();
}

void UInventoryWidget::RefreshInventory()
{
	if (!InventoryComponent)
	{
		return;
	}

	SlotGrid->ClearChildren();

	TArray<bool> OccupiedCells;
	OccupiedCells.SetNum(InventoryComponent->GridWidth * InventoryComponent->GridHeight);

	for (int32 i = 0; i < OccupiedCells.Num(); i++)
	{
		OccupiedCells[i] = false;
	}

	for (const FInventoryItemEntry& Entry : InventoryComponent->Items)
	{
		for (int32 Y = Entry.StartY; Y < Entry.StartY + Entry.Height; Y++)
		{
			for (int32 X = Entry.StartX; X < Entry.StartX + Entry.Width; X++)
			{
				const int32 Index = Y * InventoryComponent->GridWidth + X;

				if (OccupiedCells.IsValidIndex(Index))
				{
					OccupiedCells[Index] = true;
				}
			}
		}
	}

	for (int32 Y = 0; Y < InventoryComponent->GridHeight; Y++)
	{
		for (int32 X = 0; X < InventoryComponent->GridWidth; X++)
		{
			UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), InventorySlotWidgetClass);

			if (!SlotWidget)
			{
				continue;
			}

			SlotWidget->InitSlot(X, Y);

			const int32 CellIndex = Y * InventoryComponent->GridWidth + X;
			SlotWidget->SetOccupied(OccupiedCells.IsValidIndex(CellIndex) && OccupiedCells[CellIndex]);

			SlotWidget->OnInventorySlotClicked.AddDynamic(this, &UInventoryWidget::OnSlotClicked);

			UUniformGridSlot* GridSlot = SlotGrid->AddChildToUniformGrid(SlotWidget, Y, X);

			if (GridSlot)
			{
				GridSlot->SetHorizontalAlignment(HAlign_Fill);
				GridSlot->SetVerticalAlignment(VAlign_Fill);
			}
		}
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
			const float PosX = Entry.StartX * SlotSize;
			const float PosY = Entry.StartY * SlotSize;

			const float SizeX = Entry.Width * SlotSize;
			const float SizeY = Entry.Height * SlotSize;

			CanvasSlot->SetPosition(FVector2D(PosX, PosY));
			CanvasSlot->SetSize(FVector2D(SizeX, SizeY));

			CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			CanvasSlot->SetPosition(FVector2D(
				PosX + SizeX * 0.5f,
				PosY + SizeY * 0.5f
			));
		}
	}
}

void UInventoryWidget::OnSlotClicked(int32 X, int32 Y)
{

}

void UInventoryWidget::SelectItem(UItemData* ItemData)
{
	if (!ItemData)
	{
		return;
	}

	SelectedItem = ItemData;

	if (ItemNameText)
	{
		ItemNameText->SetText(ItemData->ItemName);
	}

	if (ItemDescriptionText)
	{
		ItemDescriptionText->SetText(ItemData->ItemDescription);
	}
}

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::SpaceBar)
	{
		OpenInspectWindow();
		return FReply::Handled();
	}

	if (InKeyEvent.GetKey() == EKeys::Tab)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				if (AProject_SmileCharacter* Player = Cast<AProject_SmileCharacter>(Pawn))
				{
					Player->ToggleInventoryUI();
					return FReply::Handled();
				}
			}
		}
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UInventoryWidget::OpenInspectWindow()
{
	if (!SelectedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("조사할 아이템이 선택되지 않았습니다."));
		return;
	}

	if (!InspectWidgetClass)
	{
		return;
	}

	if (!InspectWidget)
	{
		InspectWidget = CreateWidget<UItemInspectWidget>(GetWorld(), InspectWidgetClass);
	}

	if (InspectWidget)
	{
		InspectWidget->SetItemData(SelectedItem);
		InspectWidget->AddToViewport(300);

		// 인벤토리 닫기
		RemoveFromParent();
	}

	UE_LOG(LogTemp, Warning, TEXT("아이템 조사창 열기: %s"), *SelectedItem->ItemName.ToString());
}