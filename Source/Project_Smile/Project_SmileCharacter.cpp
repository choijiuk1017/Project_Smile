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

#include "Project_Smile/Widget/PuzzleHintDialogue.h"

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

void AProject_SmileCharacter::CapturePhoto()
{
	if (!SceneCaptureComp || !CaptureRenderTarget)
	{
		return;
	}

	//SceneCaptureComp->CaptureScene();

	SendCaptureToServer();
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
