// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "ExiaCharacterBase.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_CheckDefense.generated.h"

UCLASS()
class SDGUNDAMBA_API UBTService_CheckDefense : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_CheckDefense();

protected:
	// 서비스가 주기적으로 실행할 로직 (Tick 같은 역할)
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 에디터에서 조절할 가드 확률 (0~100)
	UPROPERTY(EditAnywhere, Category = "AI")
	float GuardChance = 60.0f;
	
	//TODO 지능적인 가드 시간 조율 고도화 필수 상황따라 다르게 가드를 펼치도록
	UPROPERTY(EditAnywhere, Category = "AI")
	float MinGuardDuration = 1.0f;
	
	float CurrentGuardTime = 0.0f;
};