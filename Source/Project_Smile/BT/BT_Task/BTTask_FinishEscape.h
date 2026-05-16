// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FinishEscape.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_SMILE_API UBTTask_FinishEscape : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FinishEscape();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory
	) override;
};
