// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/InteractableActor.h"


#include "Project_SmileCharacter.h"
// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableActor::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProject_SmileCharacter* Player = Cast<AProject_SmileCharacter>(OtherActor);

	if (Player)
	{
		bIsPlayerInRange = true;
		Player->SetCurrentInteractableActor(this);
	}
}

void AInteractableActor::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AProject_SmileCharacter* Player = Cast<AProject_SmileCharacter>(OtherActor);

	if (Player)
	{
		bIsPlayerInRange = false;
		Player->ClearCurrentInteractableActor(this);
	}
}

