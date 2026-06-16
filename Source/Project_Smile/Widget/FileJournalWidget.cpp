// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/FileJournalWidget.h"

#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/GameplayStatics.h"

void UFileJournalWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CachedPlayer = Cast<AProject_SmileCharacter>(
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)
	);

	RefreshFileList();
}

void UFileJournalWidget::RefreshFileList()
{
	if (!FileListBox || !CachedPlayer)
	{
		return;
	}

	FileListBox->ClearChildren();
	FileButtons.Empty();
	CachedFiles.Empty();

	const TArray<FInvestigationFile>& Files = CachedPlayer->GetInvestigationFiles();
	CachedFiles = Files;

	if (EmptyText)
	{
		EmptyText->SetVisibility(Files.Num() == 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	for (int32 i = 0; i < CachedFiles.Num(); i++)
	{
		AddFileButton(CachedFiles[i], i);
	}
}

void UFileJournalWidget::AddFileButton(const FInvestigationFile& FileData, int32 Index)
{
	if (!FileListBox)
	{
		return;
	}

	UButton* FileButton = NewObject<UButton>(this);
	UTextBlock* ButtonText = NewObject<UTextBlock>(this);

	ButtonText->SetText(FText::FromString(FileData.Title));
	ButtonText->SetJustification(ETextJustify::Center);

	FileButton->AddChild(ButtonText);
	FileButton->OnClicked.AddDynamic(this, &UFileJournalWidget::OnFileButtonClicked);

	FileButtons.Add(FileButton);

	UVerticalBoxSlot* NewSlot = FileListBox->AddChildToVerticalBox(FileButton);

	if (NewSlot)
	{
		NewSlot->SetPadding(FMargin(0.f, 4.f, 0.f, 4.f));
	}
}

void UFileJournalWidget::OnFileButtonClicked()
{
	for (int32 i = 0; i < FileButtons.Num(); i++)
	{
		if (FileButtons[i] && FileButtons[i]->IsHovered())
		{
			if (CachedFiles.IsValidIndex(i))
			{
				ShowFileContent(CachedFiles[i]);
			}

			return;
		}
	}
}

void UFileJournalWidget::ShowFileContent(const FInvestigationFile& FileData)
{
	if (!FileContentText)
	{
		return;
	}

	FString Content = FString::Printf(
		TEXT("사건 제목\n")
		TEXT("%s\n\n")

		TEXT("위치\n")
		TEXT("%s\n\n")

		TEXT("관찰 정보\n")
		TEXT("%s\n\n")

		TEXT("추론\n")
		TEXT("%s\n\n")

		TEXT("결론\n")
		TEXT("%s"),
		*FileData.Title,
		*GetAreaDisplayName(FileData.Area),
		*FileData.Observation,
		*FileData.Reasoning,
		*FileData.Hint
	);

	FileContentText->SetText(FText::FromString(Content));
}

FString UFileJournalWidget::GetAreaDisplayName(const FString& AreaId)
{
	if (AreaId == "TutorialZone")
	{
		return TEXT("격리 병동");
	}

	if (AreaId == "MainHall")
	{
		return TEXT("중앙 로비");
	}

	if (AreaId == "LabZone")
	{
		return TEXT("생체 실험동");
	}

	return AreaId;
}