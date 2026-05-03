// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/DoorActor.h"

#include "Components/BoxComponent.h"
#include "Engine/Engine.h"

#include "Project_SmileCharacter.h"
#include "Component/InventoryComponent.h"
// Sets default values
ADoorActor::ADoorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrameMesh"));
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimelineComp"));
	DoorProxVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorProxVolume"));

	DoorFrame->SetupAttachment(RootComponent);

	Door->AttachToComponent(DoorFrame, FAttachmentTransformRules::KeepRelativeTransform);

	DoorProxVolume->AttachToComponent(DoorFrame, FAttachmentTransformRules::KeepRelativeTransform);

	DoorLamp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorLamp"));
	DoorLamp->SetupAttachment(DoorFrame);
}

// Called when the game starts or when spawned
void ADoorActor::BeginPlay()
{
	Super::BeginPlay();

	UpdateFunctionFloat.BindDynamic(this, &ADoorActor::UpdateTimelineComp);

	if (DoorTimelineFloatCurve)
	{
		DoorTimelineComp->AddInterpFloat(DoorTimelineFloatCurve, UpdateFunctionFloat);
	}
	
	DoorProxVolume->OnComponentBeginOverlap.AddDynamic(this, &ADoorActor::OnOverlapBegin);
	DoorProxVolume->OnComponentEndOverlap.AddDynamic(this, &ADoorActor::OnOverlapEnd);

	if (DoorLamp && LockedMaterial)
	{
		DoorLamp->SetMaterial(1, LockedMaterial);
	}

}

// Called every frame
void ADoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoorActor::UpdateTimelineComp(float Output)
{
	FRotator DoorNewRotation = FRotator(0.0f, Output, 0.0f);

	Door->SetRelativeRotation(DoorNewRotation);
}


void ADoorActor::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProject_SmileCharacter* Player = Cast<AProject_SmileCharacter>(OtherActor);

	if (Player)
	{
		bIsPlayerInRange = true;
		Player->SetCurrentInteractDoor(this);
	}
}

void ADoorActor::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AProject_SmileCharacter* Player = Cast<AProject_SmileCharacter>(OtherActor);

	if (Player)
	{
		bIsPlayerInRange = false;
		Player->ClearCurrentInteractDoor(this);
	}
}

void ADoorActor::Interact(AActor* Interactor)
{
	if (bIsOpen)
	{

		return;
	}

	if (!Interactor)
	{
		return;
	}

	UInventoryComponent* Inventory = Interactor->FindComponentByClass<UInventoryComponent>();

	if (!Inventory)
	{
		return;
	}

	if (!Inventory->HasItemByID(RequiredItemID))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("필요한 키카드가 없습니다."));
		}

		if (AProject_SmileCharacter* Player = Cast<AProject_SmileCharacter>(Interactor))
		{
			Player->UpdateInteractionText(FailText);
		}

		return;
	}

	if (bConsumeItemOnOpen)
	{
		Inventory->RemoveItemByID(RequiredItemID);
	}

	bIsOpen = true;

	if (DoorProxVolume)
	{
		DoorProxVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DoorProxVolume->SetGenerateOverlapEvents(false);
	}

	DoorTimelineComp->Play();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("문이 열렸습니다."));
	}

	if (AProject_SmileCharacter* Player = Cast<AProject_SmileCharacter>(Interactor))
	{
		Player->UpdateInteractionText(SuccessText);
	}

	if (DoorLamp && UnlockedMaterial)
	{
		DoorLamp->SetMaterial(1, UnlockedMaterial);
	}
}
