// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GenericTeamAgentInterface.h"
#include "GundamAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

/**
 * 
 */
UCLASS()
class SDGUNDAMBA_API AGundamAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGundamAIController();
	
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);
	
	//TODO Ai 컴포넌트
	// 비헤이비어 트리를 실행할 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTreeComponent* BehaviorTreeComp;

	// AI의 기억장치 (변수 저장)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComp;

	// AI의 감각 기관 (시각)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* PerceptionComp;

	// 시각 설정 (시야각, 감지 거리 등)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	// --- [데이터 에셋 키 이름] ---
	// 블랙보드에서 쓸 변수 이름들을 미리 정의
	static const FName Key_TargetActor; // 타겟
	static const FName Key_HasLineOfSight; // 시야 확보 여부
};
