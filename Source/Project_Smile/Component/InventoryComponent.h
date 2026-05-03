// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/InventoryTypes.h"
#include "Delegates/DelegateCombinations.h"


#include "InventoryComponent.generated.h"


class UItemData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryMessage, FText, Message);



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_SMILE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 GridWidth = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 GridHeight = 3;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryItemEntry> Items;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryMessage OnInventoryMessage;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool TryAddItem(UItemData* ItemData);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool CanPlaceItem(UItemData* ItemData, int32 StartX, int32 StartY) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItemByID(FName ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemByID(FName ItemID);

private:
	TArray<int32> GridCells;

	void InitializeGrid();

	bool FindEmptyPosition(UItemData* ItemData, int32& OutX, int32& OutY) const;

	void PlaceItem(UItemData* ItemData, int32 StartX, int32 StartY);

	int32 GetIndex(int32 X, int32 Y) const;

	void RebuildGrid();
};
