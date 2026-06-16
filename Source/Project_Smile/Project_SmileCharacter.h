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
class UUserWidget;
struct FInputActionValue;
class UFileJournalWidget;

class UPuzzleHintDialogue;
class UCaptureSelection;
class UInteractionText;

class UInventoryComponent;
class AItemActor;
class UInventoryWidget;
class ADoorActor;
class AInteractableActor;

class UAnalysisLoadingWidget;


USTRUCT(BlueprintType)
struct FInvestigationFile
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Investigation File")
	int32 FileID = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Investigation File")
	FString Title;

	UPROPERTY(BlueprintReadOnly, Category = "Investigation File")
	FString Area;

	UPROPERTY(BlueprintReadOnly, Category = "Investigation File")
	FString SceneType;

	UPROPERTY(BlueprintReadOnly, Category = "Investigation File")
	FString Observation;

	UPROPERTY(BlueprintReadOnly, Category = "Investigation File")
	FString Reasoning;

	UPROPERTY(BlueprintReadOnly, Category = "Investigation File")
	FString Hint;
};


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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleInventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FileJournalAction;

public:
	AProject_SmileCharacter();

	void ToggleInventoryUI();
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

	UPROPERTY()
	UInteractionText* InteractionTextWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UInteractionText> InteractionTextWidgetClass;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	UInventoryWidget* InventoryWidget = nullptr;

	UPROPERTY()
	AItemActor* CurrentInteractItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	FString ItemInteractionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameoverWidgetClass;

	UPROPERTY()
	UUserWidget* GameoverWidgetInstance;

private:
	UPROPERTY(VisibleAnywhere, Category="Capture")
	USceneComponent* CaptureRoot;

	UPROPERTY(VisibleAnywhere, Category = "Capture")
	USceneCaptureComponent2D* SceneCaptureComp;

	UPROPERTY()
	UTextureRenderTarget2D* CaptureRenderTarget;

	UPROPERTY()
	ADoorActor* CurrentInteractDoor = nullptr;

	UPROPERTY()
	AInteractableActor* CurrentInteractableActor = nullptr;

public:

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	void SetCurrentInteractItem(AItemActor* Item);
	void ClearCurrentInteractItem(AItemActor* Item);
	void Interact();

	void SetCurrentInteractDoor(ADoorActor* Door);
	void ClearCurrentInteractDoor(ADoorActor* Door);

	void SetCurrentInteractableActor(AInteractableActor* InteractableActor);
	void ClearCurrentInteractableActor(AInteractableActor* InteractableActor);

	void UpdateInteractionText(const FString& NewText);

	void GameOver();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Director")
	FString CurrentAreaID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Director")
	FString CurrentObjectiveID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Director")
	FString LastInteractionTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Director")
	int32 HintRequestCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Director")
	float PlayTimeSeconds;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Director")
	float AreaStaySeconds;

	void SetCurrentAreaID(const FString& NewAreaID);
	void SetCurrentObjectiveID(const FString& NewObjectiveID);
	void SetLastInteractionTarget(const FString& NewTarget);

	UPROPERTY(BlueprintReadOnly, Category = "Investigation File")
	TArray<FInvestigationFile> InvestigationFiles;

	UPROPERTY(BlueprintReadOnly, Category = "Investigation File")
	int32 InvestigationFileCounter = 0;

	UFUNCTION(BlueprintCallable, Category = "Investigation File")
	void AddInvestigationFile(
		const FString& Title,
		const FString& Area,
		const FString& SceneType,
		const FString& Observation,
		const FString& Reasoning,
		const FString& Hint
	);

	UFUNCTION(BlueprintCallable, Category = "Investigation File")
	const TArray<FInvestigationFile>& GetInvestigationFiles() const;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UFileJournalWidget> FileJournalWidgetClass;

	UPROPERTY()
	UFileJournalWidget* FileJournalWidget;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	bool bIsFileJournalOpen = false;

	void ToggleFileJournal();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UAnalysisLoadingWidget> AnalysisLoadingWidgetClass;

	UPROPERTY()
	UAnalysisLoadingWidget* AnalysisLoadingWidget;

	void ShowAnalysisLoadingUI();
	void HideAnalysisLoadingUI();
};

