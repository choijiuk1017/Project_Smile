// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MonsterAIController.h"

#include "Character/MonsterCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

}

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AMonsterCharacter* Monster = Cast<AMonsterCharacter>(InPawn);
	if (Monster && Monster->BehaviorTreeAsset)
	{
		RunBehaviorTree(Monster->BehaviorTreeAsset);

		CachedBehaviorTree = Cast<UBehaviorTreeComponent>(BrainComponent);
	}
}

void AMonsterAIController::PauseAI()
{
	if (BrainComponent)
	{
		BrainComponent->StopLogic(TEXT("Parried"));
	}
}

void AMonsterAIController::ResumeAI()
{
	AMonsterCharacter* Monster = Cast<AMonsterCharacter>(GetPawn());
	if (Monster && Monster->BehaviorTreeAsset)
	{
		RunBehaviorTree(Monster->BehaviorTreeAsset);
		CachedBehaviorTree = Cast<UBehaviorTreeComponent>(BrainComponent);
	}
}