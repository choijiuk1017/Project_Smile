// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_SmileCharacter.h"
#include "FileJournalWidget.generated.h"

class UVerticalBox;
class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class PROJECT_SMILE_API UFileJournalWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void RefreshFileList();

protected:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* FileListBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FileContentText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EmptyText;

private:
	UPROPERTY()
	AProject_SmileCharacter* CachedPlayer;

	UPROPERTY()
	TArray<UButton*> FileButtons;

	UPROPERTY()
	TArray<FInvestigationFile> CachedFiles;

	void AddFileButton(const FInvestigationFile& FileData, int32 Index);
	void ShowFileContent(const FInvestigationFile& FileData);

	UFUNCTION()
	void OnFileButtonClicked();

	FString GetAreaDisplayName(const FString& AreaId);
};
