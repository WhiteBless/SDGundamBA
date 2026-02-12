// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Evade.generated.h"

/**
 * 
 */
UCLASS()
class SDGUNDAMBA_API UBTTask_Evade : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Evade();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 회피 동작 지속 시간 (이 시간 동안 부스트 모션 유지)
	UPROPERTY(EditAnywhere, Category = "AI")
	float EvadeDuration = 0.7f;

	// 회피 강도 (얼마나 세게 튀어 나갈지)
	UPROPERTY(EditAnywhere, Category = "AI")
	float EvadeStrength = 3000.0f;

	// 내부 타이머
	float ElapsedTime = 0.0f;
};
