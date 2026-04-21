// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

#include "Project_SmileCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

class UPuzzleHintDialogue;
class UCaptureSelection;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AProject_SmileCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CaptureAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleSelectionUIAction;



public:
	AProject_SmileCharacter();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void CapturePhoto();

	void ToggleCaptureSelectionUI();

	void SendCaptureToServer();

	void SendCaptureToServerWithSelection();

	bool ConvertRenderTargetToPNGBytes(UTextureRenderTarget2D* RenderTarget, TArray<uint8>& OutPNGData);

	bool ConvertRenderTargetToCroppedPNGBytes(
		UTextureRenderTarget2D* RenderTarget,
		TArray<uint8>& OutPNGData,
		int32 CropX,
		int32 CropY,
		int32 CropWidth,
		int32 CropHeight);

protected:
	// APawn interface
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	// End of APawn interface

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPuzzleHintDialogue> PuzzleHintDialogueClass;

	UPROPERTY()
	UPuzzleHintDialogue* PuzzleHintDialogueWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UCaptureSelection> CaptureSelectionWidgetClass;

	UPROPERTY()
	UCaptureSelection* CaptureSelectionWidgetInstance = nullptr;


private:
	UPROPERTY(VisibleAnywhere, Category="Capture")
	USceneComponent* CaptureRoot;

	UPROPERTY(VisibleAnywhere, Category = "Capture")
	USceneCaptureComponent2D* SceneCaptureComp;

	UPROPERTY()
	UTextureRenderTarget2D* CaptureRenderTarget;


public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ID")
	FString CurrentAreaID;

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

