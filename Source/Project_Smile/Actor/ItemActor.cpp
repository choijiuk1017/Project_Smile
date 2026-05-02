// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/ItemActor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

#include "Component/InventoryComponent.h"
#include "Data/ItemData.h"


// Sets default values
AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(SceneRoot);

	InteractionRange = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionRange"));
	InteractionRange->SetupAttachment(SceneRoot);
	InteractionRange->SetSphereRadius(150.0f);

}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemActor::Interact(AActor* Interactor)
{
	if (!Interactor || !ItemData)
	{
		return;
	}

	UInventoryComponent* Inventory = Interactor->FindComponentByClass<UInventoryComponent>();

	if (!Inventory)
	{
		return;
	}

	const bool bAdded = Inventory->TryAddItem(ItemData);

	if (bAdded)
	{
		Destroy();
	}
}

