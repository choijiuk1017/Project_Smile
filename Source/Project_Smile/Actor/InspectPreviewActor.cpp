// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/InspectPreviewActor.h"

#include "Components/StaticMeshComponent.h"

AInspectPreviewActor::AInspectPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	PreviewMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMeshComponent"));
	PreviewMeshComponent->SetupAttachment(SceneRoot);

	PreviewMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AInspectPreviewActor::SetPreviewMesh(UStaticMesh* InMesh)
{
	if (!PreviewMeshComponent)
	{
		return;
	}

	PreviewMeshComponent->SetStaticMesh(InMesh);

	if (InMesh)
	{
		PreviewMeshComponent->SetRelativeLocation(FVector::ZeroVector);
		PreviewMeshComponent->SetRelativeRotation(FRotator::ZeroRotator);
		PreviewMeshComponent->SetRelativeScale3D(FVector(1.0f));
	}
}

void AInspectPreviewActor::AddPreviewRotation(float DeltaYaw, float DeltaPitch)
{
	if (!PreviewMeshComponent)
	{
		return;
	}

	FRotator CurrentRot = PreviewMeshComponent->GetRelativeRotation();

	CurrentRot.Yaw += DeltaYaw;
	CurrentRot.Pitch = FMath::Clamp(CurrentRot.Pitch + DeltaPitch, -80.0f, 80.0f);

	PreviewMeshComponent->SetRelativeRotation(CurrentRot);
}

