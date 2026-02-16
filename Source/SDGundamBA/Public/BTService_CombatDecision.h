// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_CombatDecision.generated.h"

/**
 * 
 */
UCLASS()
class SDGUNDAMBA_API UBTService_CombatDecision : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTService_CombatDecision();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	float LastEvadeTime = 0.0f;
	
	// 공격 사거리 (이 안으로 들어오면 칼질 시작)
	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 300.0f;

	// 추적 개시 거리 (이보다 멀면 부스트 킴)
	UPROPERTY(EditAnywhere, Category = "AI")
	float ChaseRange = 800.0f;

	// 회피 확률 (플레이어 공격 시)
	UPROPERTY(EditAnywhere, Category = "AI")
	float EvadeChance = 30.0f;

	// 가드 확률
	UPROPERTY(EditAnywhere, Category = "AI")
	float GuardChance = 25.0f;
};
