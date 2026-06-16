// Fill out your copyright notice in the Description page of Project Settings.


#include "Trigger/AreaTriggerActor.h"
#include "Components/BoxComponent.h"
#include "Project_Smile/Project_SmileCharacter.h"

AAreaTriggerActor::AAreaTriggerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	AreaTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaTrigger"));
	SetRootComponent(AreaTrigger);

	AreaTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	AreaTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	AreaID = TEXT("UnknownArea");
}

void AAreaTriggerActor::BeginPlay()
{
	Super::BeginPlay();

	AreaTrigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&AAreaTriggerActor::OnAreaBeginOverlap
	);
}

void AAreaTriggerActor::OnAreaBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	AProject_SmileCharacter* PlayerCharacter = Cast<AProject_SmileCharacter>(OtherActor);

	if (!PlayerCharacter)
	{
		return;
	}

	PlayerCharacter->SetCurrentAreaID(AreaID);

	UE_LOG(LogTemp, Warning, TEXT("CurrentAreaID Changed: %s"), *AreaID);
}