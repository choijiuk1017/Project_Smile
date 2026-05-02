// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"

#include "Data/ItemData.h"

#include "Engine/Engine.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeGrid();
	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UInventoryComponent::InitializeGrid()
{
	GridCells.Empty();
	GridCells.SetNum(GridWidth * GridHeight);

	for (int32 i = 0; i < GridCells.Num(); i++)
	{
		GridCells[i] = -1;
	}
}

int32 UInventoryComponent::GetIndex(int32 X, int32 Y) const
{
	return Y * GridWidth + X;
}

bool UInventoryComponent::TryAddItem(UItemData* ItemData)
{
	if (!ItemData)
	{
		return false;
	}

	int32 FoundX = 0;
	int32 FoundY = 0;

	if (!FindEmptyPosition(ItemData, FoundX, FoundY))
	{
		const FText Message = FText::FromString(TEXT("인벤토리 공간이 부족합니다."));
		OnInventoryMessage.Broadcast(Message);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, Message.ToString());
		}

		return false;
	}

	PlaceItem(ItemData, FoundX, FoundY);

	FText Message = FText::Format(
		FText::FromString(TEXT("{0} 획득")),
		FText::FromName(ItemData->ItemID)
	);

	OnInventoryMessage.Broadcast(Message);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, Message.ToString());
	}

	OnInventoryChanged.Broadcast();

	return true;
}

bool UInventoryComponent::FindEmptyPosition(UItemData* ItemData, int32& OutX, int32& OutY) const
{
	if (!ItemData)
	{
		return false;
	}

	for (int32 Y = 0; Y < GridHeight; Y++)
	{
		for (int32 X = 0; X < GridWidth; X++)
		{
			if (CanPlaceItem(ItemData, X, Y))
			{
				OutX = X;
				OutY = Y;
				return true;
			}
		}
	}

	return false;
}

bool UInventoryComponent::CanPlaceItem(UItemData* ItemData, int32 StartX, int32 StartY) const
{
	if (!ItemData)
	{
		return false;
	}

	const int32 ItemWidth = ItemData->GridWidth;
	const int32 ItemHeight = ItemData->GridHeight;

	if (StartX + ItemWidth > GridWidth)
	{
		return false;
	}

	if (StartY + ItemHeight > GridHeight)
	{
		return false;
	}

	for (int32 Y = StartY; Y < StartY + ItemHeight; Y++)
	{
		for (int32 X = StartX; X < StartX + ItemWidth; X++)
		{
			const int32 Index = GetIndex(X, Y);

			if (!GridCells.IsValidIndex(Index))
			{
				return false;
			}

			if (GridCells[Index] != -1)
			{
				return false;
			}
		}
	}

	return true;
}

void UInventoryComponent::PlaceItem(UItemData* ItemData, int32 StartX, int32 StartY)
{
	if (!ItemData)
	{
		return;
	}

	FInventoryItemEntry NewEntry;
	NewEntry.ItemData = ItemData;
	NewEntry.StartX = StartX;
	NewEntry.StartY = StartY;
	NewEntry.Width = ItemData->GridWidth;
	NewEntry.Height = ItemData->GridHeight;

	const int32 NewItemIndex = Items.Num();
	Items.Add(NewEntry);

	for (int32 Y = StartY; Y < StartY + ItemData->GridHeight; Y++)
	{
		for (int32 X = StartX; X < StartX + ItemData->GridWidth; X++)
		{
			const int32 Index = GetIndex(X, Y);

			if (GridCells.IsValidIndex(Index))
			{
				GridCells[Index] = NewItemIndex;
			}
		}
	}
}
