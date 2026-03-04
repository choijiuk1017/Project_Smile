// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project_SmilePickUpComponent.h"

UProject_SmilePickUpComponent::UProject_SmilePickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UProject_SmilePickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UProject_SmilePickUpComponent::OnSphereBeginOverlap);
}

void UProject_SmilePickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	AProject_SmileCharacter* Character = Cast<AProject_SmileCharacter>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
