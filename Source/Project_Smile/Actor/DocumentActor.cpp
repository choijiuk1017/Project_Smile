#include "Actor/DocumentActor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

#include "Data/ItemData.h"
#include "Widget/DocumentWidget.h"

ADocumentActor::ADocumentActor()
{
	PrimaryActorTick.bCanEverTick = false;

	DocumentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DocumentMesh"));
	RootComponent = DocumentMesh;

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);

	DocumentCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DocumentCamera"));
	DocumentCamera->SetupAttachment(RootComponent);
	DocumentCamera->bAutoActivate = false;

	InteractionBox->SetBoxExtent(FVector(80.0f, 80.0f, 80.0f));
	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionBox->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	InteractionText = TEXT("문서 읽기");

	CurrentDocumentWidget = nullptr;
	PreviousViewTarget = nullptr;
	CachedPlayerController = nullptr;
}

void ADocumentActor::BeginPlay()
{
	Super::BeginPlay();

	if (InteractionBox)
	{
		InteractionBox->OnComponentBeginOverlap.AddDynamic(
			this,
			&ADocumentActor::OnOverlapBegin
		);

		InteractionBox->OnComponentEndOverlap.AddDynamic(
			this,
			&ADocumentActor::OnOverlapEnd
		);
	}
}

void ADocumentActor::Interact(AActor* Interactor)
{
	if (!Interactor)
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!PC)
	{
		return;
	}

	CachedPlayerController = PC;
	PreviousViewTarget = PC->GetViewTarget();

	if (DocumentCamera)
	{
		PC->SetViewTargetWithBlend(this, CameraBlendTime);
	}

	PC->SetIgnoreMoveInput(true);
	PC->SetIgnoreLookInput(true);

	if (DocumentWidgetClass)
	{
		CurrentDocumentWidget = CreateWidget<UDocumentWidget>(PC, DocumentWidgetClass);

		if (CurrentDocumentWidget)
		{
			CurrentDocumentWidget->SetDocumentData(DocumentData);
			CurrentDocumentWidget->SetOwningDocumentActor(this);
			CurrentDocumentWidget->AddToViewport();

			CurrentDocumentWidget->SetIsFocusable(true);

			PC->bShowMouseCursor = false;
			PC->SetShowMouseCursor(false);

			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(true);

			PC->SetInputMode(InputMode);
			PC->SetShowMouseCursor(false);

			CurrentDocumentWidget->SetFocus();
			CurrentDocumentWidget->SetKeyboardFocus();

			InteractionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			InteractionBox->SetGenerateOverlapEvents(false);
		}
	}
}

void ADocumentActor::CloseDocument()
{
	APlayerController* PC = CachedPlayerController;

	if (!PC)
	{
		PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}

	if (CurrentDocumentWidget)
	{
		CurrentDocumentWidget->RemoveFromParent();
		CurrentDocumentWidget = nullptr;
	}

	if (PC)
	{
		if (PreviousViewTarget)
		{
			PC->SetViewTargetWithBlend(PreviousViewTarget, CameraBlendTime);
		}

		PC->ResetIgnoreMoveInput();
		PC->ResetIgnoreLookInput();

		PC->bShowMouseCursor = false;

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}

	PreviousViewTarget = nullptr;
	CachedPlayerController = nullptr;
}
void ADocumentActor::CalcCamera(
	float DeltaTime,
	FMinimalViewInfo& OutResult
)
{
	if (DocumentCamera)
	{
		DocumentCamera->GetCameraView(DeltaTime, OutResult);
		return;
	}

	Super::CalcCamera(DeltaTime, OutResult);
}