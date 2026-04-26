// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project_SmileCharacter.h"


#include "Engine/World.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"


#include "Project_SmileProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

#include "Project_Smile/Widget/PuzzleHintDialogue.h"
#include "Project_Smile/Widget/CaptureSelection.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AProject_SmileCharacter

AProject_SmileCharacter::AProject_SmileCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));


	PrimaryActorTick.bCanEverTick = true;

	CaptureRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CaptureRoot"));
	CaptureRoot->SetupAttachment(GetRootComponent());

	SceneCaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComp"));
	SceneCaptureComp->SetupAttachment(CaptureRoot);

	SceneCaptureComp->bCaptureEveryFrame = false;
	SceneCaptureComp->bCaptureOnMovement = false;
	SceneCaptureComp->ProjectionType = ECameraProjectionMode::Perspective;
	SceneCaptureComp->FOVAngle = 90.0f;

	CaptureRoot->SetRelativeLocation(FVector(0.f, 0.f, 60.f));

	CurrentAreaID = "TutorialZone";

}


void AProject_SmileCharacter::BeginPlay()
{
	Super::BeginPlay();

	CaptureRenderTarget = NewObject<UTextureRenderTarget2D>(this);
	if (CaptureRenderTarget)
	{
		CaptureRenderTarget->InitAutoFormat(1024, 1024);
		CaptureRenderTarget->ClearColor = FLinearColor::Black;
		CaptureRenderTarget->TargetGamma = 2.2f;
		CaptureRenderTarget->UpdateResourceImmediate(true);

		SceneCaptureComp->TextureTarget = CaptureRenderTarget;
	}

	if (SceneCaptureComp)
	{
		SceneCaptureComp->bCaptureEveryFrame = false;
		SceneCaptureComp->bCaptureOnMovement = false;
		SceneCaptureComp->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

		SceneCaptureComp->PostProcessSettings.bOverride_AutoExposureMethod = true;
		SceneCaptureComp->PostProcessSettings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;

		SceneCaptureComp->PostProcessSettings.bOverride_AutoExposureBias = true;
		SceneCaptureComp->PostProcessSettings.AutoExposureBias = 3.0f;
	}
}

void  AProject_SmileCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//////////////////////////////////////////////////////////////////////////// Input

void AProject_SmileCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AProject_SmileCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProject_SmileCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProject_SmileCharacter::Look);

		EnhancedInputComponent->BindAction(CaptureAction, ETriggerEvent::Started, this, &AProject_SmileCharacter::CapturePhoto);
		
		EnhancedInputComponent->BindAction(ToggleSelectionUIAction, ETriggerEvent::Started, this, &AProject_SmileCharacter::ToggleCaptureSelectionUI);
	
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AProject_SmileCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AProject_SmileCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AProject_SmileCharacter::ToggleCaptureSelectionUI()
{
	if (CaptureSelectionWidgetInstance)
	{
		CaptureSelectionWidgetInstance->RemoveFromParent();
		CaptureSelectionWidgetInstance = nullptr;

		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
		}

		return;
	}

	if (!CaptureSelectionWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CaptureSelectionWidgetClass is null"));
		return;
	}

	CaptureSelectionWidgetInstance = CreateWidget<UCaptureSelection>(GetWorld(), CaptureSelectionWidgetClass);
	if (CaptureSelectionWidgetInstance)
	{
		CaptureSelectionWidgetInstance->AddToViewport(200);

		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(CaptureSelectionWidgetInstance->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
		}

		CaptureSelectionWidgetInstance->SetKeyboardFocus();
	}
}

void AProject_SmileCharacter::CapturePhoto()
{
	if (!SceneCaptureComp || !CaptureRenderTarget)
	{
		return;
	}

	if (CaptureSelectionWidgetInstance)
	{
		SendCaptureToServerWithSelection();
	}
	else
	{
		SendCaptureToServer();
	}
}

bool AProject_SmileCharacter::ConvertRenderTargetToPNGBytes(UTextureRenderTarget2D* RenderTarget, TArray<uint8>& OutPNGData)
{
	if (!RenderTarget)
	{
		return false;
	}

	// GPU -> CPU 데이터 접근
	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RenderTargetResource)
	{
		return false;
	}

	TArray<FColor> PixelData;
	if (!RenderTargetResource->ReadPixels(PixelData)) // 화면 데이터를 FColor 배열로 전환
	{
		return false;
	}

	const int32 Width = RenderTarget->SizeX;
	const int32 Height = RenderTarget->SizeY;

	IImageWrapperModule& ImageWrapperModule =
		FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	if (!ImageWrapper.IsValid())
	{
		return false;
	}

	// PNG 압축
	if (!ImageWrapper->SetRaw(
		PixelData.GetData(),
		PixelData.Num() * sizeof(FColor),
		Width,
		Height,
		ERGBFormat::BGRA,
		8))
	{
		return false;
	}

	// 압축률 100, 최고 품질
	const TArray64<uint8>& CompressedData = ImageWrapper->GetCompressed(100);

	// 서버로 보낼 데이터, 리셋 후 새로 보냄
	OutPNGData.Reset();
	OutPNGData.Append(CompressedData.GetData(), CompressedData.Num());

	return true;
}

bool AProject_SmileCharacter::ConvertRenderTargetToCroppedPNGBytes(
	UTextureRenderTarget2D* RenderTarget,
	TArray<uint8>& OutPNGData,
	int32 CropX,
	int32 CropY,
	int32 CropWidth,
	int32 CropHeight)
{
	if (!RenderTarget)
	{
		return false;
	}

	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RenderTargetResource)
	{
		return false;
	}

	TArray<FColor> FullPixelData;
	if (!RenderTargetResource->ReadPixels(FullPixelData))
	{
		return false;
	}

	const int32 FullWidth = RenderTarget->SizeX;
	const int32 FullHeight = RenderTarget->SizeY;

	CropX = FMath::Clamp(CropX, 0, FullWidth - 1);
	CropY = FMath::Clamp(CropY, 0, FullHeight - 1);
	CropWidth = FMath::Clamp(CropWidth, 1, FullWidth - CropX);
	CropHeight = FMath::Clamp(CropHeight, 1, FullHeight - CropY);

	TArray<FColor> CroppedPixelData;
	CroppedPixelData.Reserve(CropWidth * CropHeight);

	for (int32 Y = 0; Y < CropHeight; ++Y)
	{
		for (int32 X = 0; X < CropWidth; ++X)
		{
			const int32 SrcX = CropX + X;
			const int32 SrcY = CropY + Y;
			const int32 SrcIndex = SrcY * FullWidth + SrcX;

			CroppedPixelData.Add(FullPixelData[SrcIndex]);
		}
	}

	IImageWrapperModule& ImageWrapperModule =
		FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	if (!ImageWrapper.IsValid())
	{
		return false;
	}

	if (!ImageWrapper->SetRaw(
		CroppedPixelData.GetData(),
		CroppedPixelData.Num() * sizeof(FColor),
		CropWidth,
		CropHeight,
		ERGBFormat::BGRA,
		8))
	{
		return false;
	}

	const TArray64<uint8>& CompressedData = ImageWrapper->GetCompressed(100);

	OutPNGData.Reset();
	OutPNGData.Append(CompressedData.GetData(), CompressedData.Num());

	return true;
}

void AProject_SmileCharacter::SendCaptureToServer()
{
	if (!SceneCaptureComp || !CaptureRenderTarget)
	{
		return;
	}

	SceneCaptureComp->CaptureScene();


	// 이미지 -> byte 변환
	TArray<uint8> PNGData;
	if (!ConvertRenderTargetToPNGBytes(CaptureRenderTarget, PNGData))
	{
		return;
	}

	// 언리얼 HTTP 객체 생성
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	// 요청 설정
	Request->SetURL(TEXT("http://127.0.0.1:5000/predict"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));

	Request->SetHeader(TEXT("X-Area-Id"), CurrentAreaID);

	Request->SetContent(PNGData);

	Request->OnProcessRequestComplete().BindLambda(
		[this](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!bWasSuccessful || !Response.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("HTTP request failed"));
				return;
			}

			FString ResponseString = Response->GetContentAsString();
			UE_LOG(LogTemp, Warning, TEXT("Server Response: %s"), *ResponseString);

			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				FString PredictedClass = JsonObject->GetStringField(TEXT("class"));
				double Confidence = JsonObject->GetNumberField(TEXT("confidence"));

				FString Hint = JsonObject->GetStringField(TEXT("hint")); 

				UE_LOG(LogTemp, Warning, TEXT("Predicted Class: %s"), *PredictedClass);
				UE_LOG(LogTemp, Warning, TEXT("Confidence: %f"), Confidence);
				UE_LOG(LogTemp, Warning, TEXT("Hint: %s"), *Hint); 

				if (!this->PuzzleHintDialogueClass) return;

				PuzzleHintDialogueWidget = CreateWidget<UPuzzleHintDialogue>(GetWorld(), PuzzleHintDialogueClass);

				if (PuzzleHintDialogueWidget)
				{
					PuzzleHintDialogueWidget->SetDialogue(*Hint);
					PuzzleHintDialogueWidget->AddToViewport(100);
				}
				
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response"));
			}
		});
		
	Request->ProcessRequest();
}

void AProject_SmileCharacter::SendCaptureToServerWithSelection()
{
	if (!SceneCaptureComp || !CaptureRenderTarget || !CaptureSelectionWidgetInstance)
	{
		return;
	}

	SceneCaptureComp->CaptureScene();

	FVector2D SelectionPos;
	FVector2D SelectionSize;
	CaptureSelectionWidgetInstance->GetSelectionRect(SelectionPos, SelectionSize);

	FVector2D ViewportSize(1.f, 1.f);
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	const int32 RTWidth = CaptureRenderTarget->SizeX;
	const int32 RTHeight = CaptureRenderTarget->SizeY;

	const int32 CropX = FMath::RoundToInt((SelectionPos.X / ViewportSize.X) * RTWidth);
	const int32 CropY = FMath::RoundToInt((SelectionPos.Y / ViewportSize.Y) * RTHeight);
	const int32 CropW = FMath::RoundToInt((SelectionSize.X / ViewportSize.X) * RTWidth);
	const int32 CropH = FMath::RoundToInt((SelectionSize.Y / ViewportSize.Y) * RTHeight);

	TArray<uint8> PNGData;
	if (!ConvertRenderTargetToCroppedPNGBytes(CaptureRenderTarget, PNGData, CropX, CropY, CropW, CropH))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to crop render target"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Sending PNG Size: %d bytes"), PNGData.Num());
	UE_LOG(LogTemp, Warning, TEXT("AreaID: %s"), *CurrentAreaID);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	Request->SetURL(TEXT("http://127.0.0.1:5000/predict"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
	Request->SetHeader(TEXT("X-Area-Id"), CurrentAreaID);
	Request->SetHeader(TEXT("X-Spoiler-Level"), TEXT("1"));
	Request->SetContent(PNGData);
	Request->SetTimeout(300.0f);

	Request->OnProcessRequestComplete().BindLambda(
		[this](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			UE_LOG(LogTemp, Warning, TEXT("HTTP Completed"));
			UE_LOG(LogTemp, Warning, TEXT("bWasSuccessful: %s"), bWasSuccessful ? TEXT("true") : TEXT("false"));
			UE_LOG(LogTemp, Warning, TEXT("URL: %s"), *Req->GetURL());
			UE_LOG(LogTemp, Warning, TEXT("Status: %d"), (int32)Req->GetStatus());

			if (!Response.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("Response is invalid"));
				return;
			}

			const int32 ResponseCode = Response->GetResponseCode();
			const FString ResponseString = Response->GetContentAsString();

			UE_LOG(LogTemp, Warning, TEXT("Response Code: %d"), ResponseCode);
			UE_LOG(LogTemp, Warning, TEXT("Response Body: %s"), *ResponseString);

			if (!bWasSuccessful || ResponseCode < 200 || ResponseCode >= 300)
			{
				UE_LOG(LogTemp, Error, TEXT("HTTP request failed"));
				return;
			}

			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

			if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response"));
				return;
			}

			FString ErrorMessage;
			if (JsonObject->TryGetStringField(TEXT("error"), ErrorMessage))
			{
				UE_LOG(LogTemp, Error, TEXT("Server Error: %s"), *ErrorMessage);
				return;
			}

			FString Hint;
			if (!JsonObject->TryGetStringField(TEXT("hint"), Hint))
			{
				UE_LOG(LogTemp, Error, TEXT("Hint field was not found"));
				return;
			}

			UE_LOG(LogTemp, Warning, TEXT("Hint: %s"), *Hint);

			if (!this->PuzzleHintDialogueClass)
			{
				UE_LOG(LogTemp, Error, TEXT("PuzzleHintDialogueClass is null"));
				return;
			}

			PuzzleHintDialogueWidget = CreateWidget<UPuzzleHintDialogue>(GetWorld(), PuzzleHintDialogueClass);

			if (PuzzleHintDialogueWidget)
			{
				PuzzleHintDialogueWidget->SetDialogue(*Hint);
				PuzzleHintDialogueWidget->AddToViewport(100);
			}
		});

	Request->ProcessRequest();
}