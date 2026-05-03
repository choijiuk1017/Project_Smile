// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ItemInspectWidget.h"

#include "Data/ItemData.h"
#include "Components/TextBlock.h"
#include "Actor/InspectPreviewActor.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/SceneCaptureComponent2D.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "InputCoreTypes.h"

void UItemInspectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	SetKeyboardFocus();
}

void UItemInspectWidget::NativeDestruct()
{
	DestroyPreviewScene();

	Super::NativeDestruct();
}

void UItemInspectWidget::SetItemData(UItemData* InItemData)
{
	ItemData = InItemData;

	if (!ItemData)
	{
		return;
	}

	if (ItemNameText)
	{
		ItemNameText->SetText(ItemData->ItemName);
	}

	if (ItemDescriptionText)
	{
		ItemDescriptionText->SetText(ItemData->ItemDescription);
	}

	CreatePreviewScene();
}

void UItemInspectWidget::CreatePreviewScene()
{
	DestroyPreviewScene();

	if (!GetWorld() || !ItemData || !ItemData->InspectMesh)
	{
		return;
	}

	const FVector PreviewLocation = FVector(100000.0f, 100000.0f, 100000.0f);

	PreviewActor = GetWorld()->SpawnActor<AInspectPreviewActor>(
		AInspectPreviewActor::StaticClass(),
		PreviewLocation,
		FRotator::ZeroRotator
	);

	if (PreviewActor)
	{
		PreviewActor->SetPreviewMesh(ItemData->InspectMesh);
	}

	CaptureActor = GetWorld()->SpawnActor<AActor>(
		AActor::StaticClass(),
		PreviewLocation + FVector(-100.0f, 0.0f, 0.0f),
		FRotator::ZeroRotator
	);

	if (!CaptureActor)
	{
		return;
	}

	if (!CaptureActor->GetRootComponent())
	{
		USceneComponent* Root = NewObject<USceneComponent>(CaptureActor);
		Root->RegisterComponent();
		CaptureActor->SetRootComponent(Root);
	}

	SceneCaptureComponent = NewObject<USceneCaptureComponent2D>(CaptureActor);

	if (!SceneCaptureComponent)
	{
		return;
	}

	SceneCaptureComponent->RegisterComponent();
	SceneCaptureComponent->AttachToComponent(
		CaptureActor->GetRootComponent(),
		FAttachmentTransformRules::KeepRelativeTransform
	);

	SceneCaptureComponent->SetWorldLocation(PreviewLocation + FVector(-100.0f, 0.0f, 0.0f));
	SceneCaptureComponent->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));
	SceneCaptureComponent->FOVAngle = 35.0f;

	SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;


	SceneCaptureComponent->ShowFlags.SetLighting(false);
	SceneCaptureComponent->ShowFlags.SetPostProcessing(false);
	SceneCaptureComponent->ShowFlags.SetFog(false);
	SceneCaptureComponent->ShowFlags.SetAtmosphere(false);

	SceneCaptureComponent->bCaptureEveryFrame = true;
	SceneCaptureComponent->bCaptureOnMovement = true;

	RenderTarget = NewObject<UTextureRenderTarget2D>(this);

	if (RenderTarget)
	{
		RenderTarget->InitAutoFormat(512, 512);
		RenderTarget->ClearColor = FLinearColor::Transparent;
		RenderTarget->UpdateResourceImmediate(true);

		SceneCaptureComponent->TextureTarget = RenderTarget;
	}

	ApplyRenderTargetToImage();
}

void UItemInspectWidget::ApplyRenderTargetToImage()
{
	if (!PreviewImage || !RenderTarget)
	{
		return;
	}

	FSlateBrush Brush;
	Brush.SetResourceObject(RenderTarget);
	Brush.ImageSize = FVector2D(512.0f, 512.0f);

	PreviewImage->SetBrush(Brush);
}

void UItemInspectWidget::DestroyPreviewScene()
{
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}

	if (CaptureActor)
	{
		CaptureActor->Destroy();
		CaptureActor = nullptr;
	}

	SceneCaptureComponent = nullptr;
	RenderTarget = nullptr;
}

FReply UItemInspectWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bDragging = true;
		LastMousePosition = InMouseEvent.GetScreenSpacePosition();

		return FReply::Handled().CaptureMouse(TakeWidget());
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UItemInspectWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bDragging = false;

		return FReply::Handled().ReleaseMouseCapture();
	}

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UItemInspectWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bDragging && PreviewActor)
	{
		const FVector2D CurrentMousePosition = InMouseEvent.GetScreenSpacePosition();
		const FVector2D Delta = CurrentMousePosition - LastMousePosition;

		const float RotationSpeed = 0.4f;

		PreviewActor->AddPreviewRotation(
			Delta.X * RotationSpeed,
			Delta.Y * RotationSpeed
		);

		LastMousePosition = CurrentMousePosition;

		return FReply::Handled();
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UItemInspectWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape || InKeyEvent.GetKey() == EKeys::SpaceBar)
	{
		CloseInspectWidget();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UItemInspectWidget::CloseInspectWidget()
{
	RemoveFromParent();

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);
	}
}

