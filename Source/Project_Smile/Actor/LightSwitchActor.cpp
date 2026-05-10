// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/LightSwitchActor.h"

#include "Components/SphereComponent.h"
#include "Engine/Engine.h"
#include "Engine/Light.h"
#include "Components/LightComponent.h"

#include "Project_SmileCharacter.h"
#include "Component/InventoryComponent.h"

ALightSwitchActor::ALightSwitchActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Switch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Switch"));
	SwitchProxVolume = CreateDefaultSubobject<USphereComponent>(TEXT("SwitchProxVolume"));


	Switch->SetupAttachment(RootComponent);

	SwitchProxVolume->AttachToComponent(Switch, FAttachmentTransformRules::KeepRelativeTransform);

	bConsumeItemOnInteract = false;
}

void ALightSwitchActor::BeginPlay()
{
	Super::BeginPlay();

	for (AActor* LampActor : TargetLampActors)
	{
		if (!LampActor) continue;

		TArray<ULightComponent*> LightComponents;
		LampActor->GetComponents<ULightComponent>(LightComponents);

		for (ULightComponent* LightComp : LightComponents)
		{
			if (LightComp)
			{
				LightComp->SetVisibility(false);
			}
		}
	}

	SwitchProxVolume->OnComponentBeginOverlap.AddDynamic(this, &ALightSwitchActor::OnOverlapBegin);
	SwitchProxVolume->OnComponentEndOverlap.AddDynamic(this, &ALightSwitchActor::OnOverlapEnd);

	SwitchProxVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwitchProxVolume->SetGenerateOverlapEvents(false);
}

void ALightSwitchActor::TurnOnSwitch()
{
	SwitchProxVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SwitchProxVolume->SetGenerateOverlapEvents(true);
}

void ALightSwitchActor::Interact(AActor* Interactor)
{
	if (!Interactor) return;

	if (bIsOperated) return;

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("불이 켜졌다."));

	for (AActor* LampActor : TargetLampActors)
	{
		if (!LampActor) continue;

		TArray<ULightComponent*> LightComponents;
		LampActor->GetComponents<ULightComponent>(LightComponents);

		for (ULightComponent* LightComp : LightComponents)
		{
			if (LightComp)
			{
				LightComp->SetVisibility(true);
				LightComp->SetIntensity(5000.f);
			}
		}
	}
	SwitchProxVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwitchProxVolume->SetGenerateOverlapEvents(false);
}