// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/InteractionText.h"

#include "Components/TextBlock.h"

void UInteractionText::SetText(FString Text)
{
	TXT_Text->SetText(FText::FromString(Text));
}