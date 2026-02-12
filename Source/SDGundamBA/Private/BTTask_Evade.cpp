// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Evade.h"
#include "AIController.h"
#include "ExiaAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h" // 블랙보드 사용 시 필요

UBTTask_Evade::UBTTask_Evade()
{
	NodeName = TEXT("Evade Boost");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Evade::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	AExiaAICharacter* AIChar = Cast<AExiaAICharacter>(AIC->GetPawn());

	if (!AIChar) return EBTNodeResult::Failed;
	
	AIChar->SetAICombatState(EGundamAICombatState::Boosting);

	//랜덤 방향 결정 (좌 / 우 / 후방)
	FVector Forward = AIChar->GetActorForwardVector();
	FVector Right = AIChar->GetActorRightVector();
	FVector LaunchDir = FVector::ZeroVector;

	int32 Rand = FMath::RandRange(0, 2);
	switch (Rand)
	{
	case 0: LaunchDir = -Right; break;   // 왼쪽
	case 1: LaunchDir = Right; break;    // 오른쪽
	case 2: LaunchDir = -Forward; break; // 뒤쪽
	}

	// 캐릭터 날리기 (Launch Character)
	FVector LaunchVelocity = (LaunchDir * EvadeStrength) + FVector(0, 0, 200.0f);
	AIChar->LaunchCharacter(LaunchVelocity, true, true);

	// 타이머 초기화 및 대기 시작
	ElapsedTime = 0.0f;
	
	return EBTNodeResult::InProgress; 
}

void UBTTask_Evade::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;

	// 지정된 시간이 지나면 회피 종료
	if (ElapsedTime >= EvadeDuration)
	{
		AAIController* AIC = OwnerComp.GetAIOwner();
		if (AExiaAICharacter* AIChar = Cast<AExiaAICharacter>(AIC->GetPawn()))
		{
			// 상태를 다시 일반 전투(Combat) 모드로 복구
			AIChar->SetAICombatState(EGundamAICombatState::Combat);
			
			// TODO 속도를 0으로 줄이는 부분
			AIChar->GetCharacterMovement()->StopMovementImmediately();
		}

		// 블랙보드의 '회피 제안' 키가 있다면 꺼줌
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("bSuggestedEvade"), false);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}