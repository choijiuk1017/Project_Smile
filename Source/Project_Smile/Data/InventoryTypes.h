// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.generated.h"

class UItemData;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FInventoryItemEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UItemData> ItemData = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 StartX = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 StartY = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Width = 1;

	UPROPERTY(BlueprintReadOnly)
	int32 Height = 1;
};
