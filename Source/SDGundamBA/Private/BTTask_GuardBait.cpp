// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GuardBait.h"
#include "AIController.h"
#include "ExiaAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GuardBait::UBTTask_GuardBait()
{
	NodeName = TEXT("Guard Bait");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_GuardBait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AExiaAICharacter* AIChar = Cast<AExiaAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!AIChar) return EBTNodeResult::Failed;

	// 1. 가드 시작
	AIChar->StartGuard();
	ElapsedTime = 0.0f;

	return EBTNodeResult::InProgress;
}

void UBTTask_GuardBait::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	AExiaAICharacter* AIChar = Cast<AExiaAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (!AIChar) 
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (BB)
	{
		AActor* Target = Cast<AActor>(BB->GetValueAsObject(FName("TargetActor")));
		if (Target)
		{
			// 컨트롤러가 타겟을 바라보도록 설정 (몸통 회전은 캐릭터 세팅에 따라감)
			AIC->SetFocus(Target);
		}
	}
	
	ElapsedTime += DeltaSeconds;
	
	if (ElapsedTime >= WaitTime)
	{
		AIChar->StopGuard();
		// 블랙보드 키 초기화
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("bSuggestedGuard"), false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
