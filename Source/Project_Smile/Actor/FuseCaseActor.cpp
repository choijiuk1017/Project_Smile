// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/FuseCaseActor.h"

#include "Components/SphereComponent.h"
#include "Engine/Engine.h"

#include "Project_SmileCharacter.h"
#include "Component/InventoryComponent.h"
#include "Actor/LightSwitchActor.h"

AFuseCaseActor::AFuseCaseActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CaseFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CaseFrame"));
	CaseDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CaseDoor"));
	CaseTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("CaseTimelineComp"));
	CaseProxVolume = CreateDefaultSubobject<USphereComponent>(TEXT("CaseProxVolume"));

	CaseFrame->SetupAttachment(RootComponent);

	CaseDoor->AttachToComponent(CaseFrame, FAttachmentTransformRules::KeepRelativeTransform);
	CaseProxVolume->AttachToComponent(CaseFrame, FAttachmentTransformRules::KeepRelativeTransform);

	MaxFuseNum = 6;

	for (int32 i = 0; i < MaxFuseNum; i++)
	{
		UStaticMeshComponent* FuseMesh = CreateDefaultSubobject<UStaticMeshComponent>(
			*FString::Printf(TEXT("FuseMesh_%d"), i)
		);

		FuseMesh->SetupAttachment(CaseFrame);
		FuseMeshes.Add(FuseMesh);
	}
}

void AFuseCaseActor::BeginPlay()
{
	Super::BeginPlay();

	UpdateFunctionFloat.BindDynamic(this, &AFuseCaseActor::UpdateTimelineComp);

	if (CaseTimelineFloatCurve)
	{
		CaseTimelineComp->AddInterpFloat(CaseTimelineFloatCurve, UpdateFunctionFloat);
	}

	CaseProxVolume->OnComponentBeginOverlap.AddDynamic(this, &AFuseCaseActor::OnOverlapBegin);
	CaseProxVolume->OnComponentEndOverlap.AddDynamic(this, &AFuseCaseActor::OnOverlapEnd);

	RandomizeMissingFuses();
}

void AFuseCaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFuseCaseActor::UpdateTimelineComp(float Output)
{
	FRotator CaseDoorNewRotation = FRotator(0.f, Output, 0.f);

	CaseDoor->SetRelativeRotation(CaseDoorNewRotation);
}


void AFuseCaseActor::Interact(AActor* Interactor)
{

	if (!Interactor) return;

	if (bIsOperated) return;

	InteractionText = "";

	if (bIsOpen)
	{
		UInventoryComponent* Inventory = Interactor->FindComponentByClass<UInventoryComponent>();
		 
		if (!Inventory) return;


		if (!Inventory->HasItemByID(RequiredItemID))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("퓨즈가 없다."));
			}
			if (AProject_SmileCharacter* Player = Cast<AProject_SmileCharacter>(Interactor))
			{
				Player->UpdateInteractionText(FailText);
			}
			return;
		}

		if (bConsumeItemOnInteract)
		{
			if (Inventory->RemoveItemByID(RequiredItemID))
			{
				if (EmptyFuseIndices.Num() > 0)
				{
					const int32 FilledIndex = EmptyFuseIndices[0];
					EmptyFuseIndices.RemoveAt(0);

					if (FuseMeshes.IsValidIndex(FilledIndex) && FuseMeshes[FilledIndex])
					{
						FuseMeshes[FilledIndex]->SetVisibility(true);
						FuseMeshes[FilledIndex]->SetHiddenInGame(false);
						FuseMeshes[FilledIndex]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					}
				}

				NeedFuseNum--;
			}
		}

		if (NeedFuseNum > 0 && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("퓨즈가 부족해 전원이 공급되지 않는다."));
			if (AProject_SmileCharacter* Player = Cast<AProject_SmileCharacter>(Interactor))
			{
				Player->UpdateInteractionText(FailText);
			}
			return;
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("작동한다."));

		for (UStaticMeshComponent* FuseMesh : FuseMeshes)
		{
			if (FuseMesh && OnMaterial)
			{
				FuseMesh->SetMaterial(0, OnMaterial);
			}
		}

		bIsOperated = true;

		if (CaseProxVolume)
		{
			CaseProxVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CaseProxVolume->SetGenerateOverlapEvents(false);
		}

		LightSwitch->TurnOnSwitch();

		if (AProject_SmileCharacter* Player = Cast<AProject_SmileCharacter>(Interactor))
		{
			Player->UpdateInteractionText(SuccessText);
		}
		return;
	}
	else
	{
		bIsOpen = true;

		CaseTimelineComp->Play();
	}

	
}

void AFuseCaseActor::RandomizeMissingFuses()
{
	EmptyFuseIndices.Empty();

	if (FuseMeshes.Num() < MaxFuseNum)
	{
		return;
	}

	NeedFuseNum = FMath::Clamp(NeedFuseNum, 0, MaxFuseNum);

	TArray<int32> IndexArray;

	for (int32 i = 0; i < MaxFuseNum; i++)
	{
		IndexArray.Add(i);
	}

	for (int32 i = 0; i < IndexArray.Num(); i++)
	{
		int32 RandomIndex = FMath::RandRange(i, IndexArray.Num() - 1);
		IndexArray.Swap(i, RandomIndex);
	}

	for (int32 i = 0; i < NeedFuseNum; i++)
	{
		int32 EmptyIndex = IndexArray[i];
		EmptyFuseIndices.Add(EmptyIndex);

		if (FuseMeshes[EmptyIndex])
		{
			FuseMeshes[EmptyIndex]->SetVisibility(false);
			FuseMeshes[EmptyIndex]->SetHiddenInGame(true);
			FuseMeshes[EmptyIndex]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	for (int32 i = 0; i < MaxFuseNum; i++)
	{
		if (!EmptyFuseIndices.Contains(i) && FuseMeshes[i])
		{
			FuseMeshes[i]->SetVisibility(true);
			FuseMeshes[i]->SetHiddenInGame(false);
			FuseMeshes[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}