// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_TrackDirectly.generated.h"

/**
 * 
 */
UCLASS()
class SDGUNDAMBA_API UBTTask_TrackDirectly : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_TrackDirectly();
	
protected:
	// 태스크 실행 시작
	virtual EBTNodeResult::Type ExecuteTask (UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	// 매 프레임 실행 ( 실제 이동 로직 )
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	// 목표와 얼마나 가까워지면 추적을 멈출지 거리에 대한 정의
	UPROPERTY(EditAnywhere, Category = "AI")
	float AcceptanceRadius = 250.0f;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	bool bDebugLines = false;
	
	
};
