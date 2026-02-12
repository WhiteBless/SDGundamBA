// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GuardBait.generated.h"

/**
 * 
 */
UCLASS()
class SDGUNDAMBA_API UBTTask_GuardBait : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_GuardBait();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category="AI")
	float WaitTime = 2.0f; // 가드 유지 시간
	
	float ElapsedTime = 0.0f;
};