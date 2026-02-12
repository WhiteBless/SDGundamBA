#include "BTService_CombatDecision.h"
#include "AIController.h"
#include "ExiaAICharacter.h"
#include "ExiaCharacterBase.h" // 플레이어 상태 확인용
#include "BehaviorTree/BlackboardComponent.h"
#include "GundamDataStructs.h"

UBTService_CombatDecision::UBTService_CombatDecision()
{
	NodeName = TEXT("Combat Decision Maker");
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_CombatDecision::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return;
	
	AExiaAICharacter* AIChar = Cast<AExiaAICharacter>(AIC->GetPawn());
	if (!AIChar) return;

	UE_LOG(LogTemp, Error, TEXT("AI Service Running... Dist: %f"), BB->GetValueAsFloat(TEXT("TargetDistance")));
	// 타겟 확인
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
	
	if (!Target)
	{
		AIChar->SetAICombatState(EGundamAICombatState::Waiting);
		BB->SetValueAsEnum(TEXT("AICombatState"), (uint8)EGundamAICombatState::Waiting);
		return;
	}

	// 거리 계산 및 저장
	float Dist = FVector::Dist(AIChar->GetActorLocation(), Target->GetActorLocation());
	BB->SetValueAsFloat(TEXT("TargetDistance"), Dist);

	// 플레이어 공격 여부 확인
	bool bIsPlayerAttacking = false;
	if (AExiaCharacterBase* PlayerChar = Cast<AExiaCharacterBase>(Target))
	{
		bIsPlayerAttacking = PlayerChar->bIsAttacking; 
	}

	// 상황별 판단 로직
	EGundamAICombatState NewState = AIChar->AICombatState; // 현재 상태 임시 저장

	// [상황 A] 위기 (플레이어 공격 & 근거리)
	if (bIsPlayerAttacking && Dist < 400.0f)
	{
		float Dice = FMath::RandRange(0.0f, 100.0f);

		if (Dice < EvadeChance)
		{
			BB->SetValueAsBool(TEXT("bSuggestedEvade"), true);
			// 회피 상태로 변경
			NewState = EGundamAICombatState::Evading;
		}
		else if (Dice < EvadeChance + GuardChance)
		{
			BB->SetValueAsBool(TEXT("bSuggestedGuard"), true);
			// 가드 상태로는 굳이 변경 안 해도 됨 (태스크가 함)
		}
	}
	
	// [상황 B] 거리가 멀다 (800 이상) -> 부스트 추적
	else if (Dist > ChaseRange)
	{
		NewState = EGundamAICombatState::Boosting;
	}
	
	// [상황 C] 적절한 거리 -> 일반 전투
	else
	{
		if (Dist <= 300.0f)
		{
			if (NewState == EGundamAICombatState::Boosting)
			{
				NewState = EGundamAICombatState::Combat;
				
				BB->SetValueAsEnum(TEXT("AICombatState"), (uint8)NewState);
			}
		}
		else if (NewState == EGundamAICombatState::Boosting)
		{
			if (Dist <= 300.0f)
			{
				NewState = EGundamAICombatState::Combat;
			}
		}
		else 
		{
			NewState = EGundamAICombatState::Combat;
		}
	}
	
	uint8 CurrentBBState = BB->GetValueAsEnum(TEXT("AICombatState"));
	if (CurrentBBState != (uint8)NewState)
	{
		BB->SetValueAsEnum(TEXT("AICombatState"), (uint8)NewState);
		UE_LOG(LogTemp, Warning, TEXT("Brain Sync Fix: BB Updated to %d"), (uint8)NewState);
	}

	// 캐릭터 내부 상태(몸)가 판단과 다르면 변경
	if (AIChar->AICombatState != NewState)
	{
		AIChar->SetAICombatState(NewState);
	}
}