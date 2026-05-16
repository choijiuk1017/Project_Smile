// Fill out your copyright notice in the Description page of Project Settings.


#include "BT/BT_Task/BTTask_FinishEscape.h"

#include "AIController.h"
#include "Character/MonsterCharacter.h"

UBTTask_FinishEscape::UBTTask_FinishEscape()
{
	NodeName = TEXT("Finish Escape");
}

EBTNodeResult::Type UBTTask_FinishEscape::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory
)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AMonsterCharacter* Monster = Cast<AMonsterCharacter>(AIController->GetPawn());

	if (!Monster)
	{
		return EBTNodeResult::Failed;
	}

	Monster->FinishEscape();

	return EBTNodeResult::Succeeded;
}
