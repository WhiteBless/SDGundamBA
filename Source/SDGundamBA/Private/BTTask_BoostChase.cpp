// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_BoostChase.h"
#include "AIController.h"
#include "ExiaAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_BoostChase::UBTTask_BoostChase()
{
	NodeName = TEXT("Boost Chase");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_BoostChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	AExiaAICharacter * AIChar = Cast<AExiaAICharacter>(AIC->GetPawn());
	
	if (!AIChar || !AIC) return EBTNodeResult::Failed;

	AIChar->SetAICombatState(EGundamAICombatState::Boosting);
	if (auto* MoveComp = AIChar->GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = AIChar->GetMoveSpeed() * 2.5f;
	}
	return EBTNodeResult::InProgress;
}

void UBTTask_BoostChase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	AExiaAICharacter* AIChar = Cast<AExiaAICharacter>(AIC->GetPawn());
	
	// 타겟 가져오기
	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));

	if (!Target)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 적을 향해 이동 명령 (매 프레임 갱신하여 유도력 향상)
	AIC->MoveToActor(Target, AcceptableRadius);

	// 거리 체크
	float Dist = FVector::Dist(AIChar->GetActorLocation(), Target->GetActorLocation());
	if (Dist <= AcceptableRadius)
	{
		// 도착했으면 속도 원복 후 성공 처리
		if (auto* MoveComp = AIChar->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = AIChar->GetMoveSpeed();
		}
		// 상태를 다시 되돌리기
		AIChar->SetAICombatState(EGundamAICombatState::Combat);
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
