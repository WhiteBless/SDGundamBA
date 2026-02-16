// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckDefense.h"
#include "AIController.h"
#include "ExiaAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_CheckDefense::UBTService_CheckDefense()
{
	NodeName = TEXT("Check Player Attack for Defense");
	Interval = 0.2f;
	RandomDeviation = 0.05f;

	// 가드 확률 기본값 (헤더에 선언되어 있어야 합니다)
	GuardChance = 30.0f; 
}

void UBTService_CheckDefense::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	// 블랙보드에서 타겟을 가져오기
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;
	
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(GetSelectedBlackboardKey()));
	AExiaCharacterBase* PlayerChar = Cast<AExiaCharacterBase>(TargetActor);
	
	AExiaAICharacter* AIChar = Cast<AExiaAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (PlayerChar)
	{
		bool bInAir = PlayerChar->GetCharacterMovement()->IsFalling();
		BB->SetValueAsBool(FName("bIsTargetInAir"), bInAir);
	}
	
	if (AIChar && PlayerChar)
	{
		bool bPlayerIsThreatening = PlayerChar->GetIsAttacking() || PlayerChar->IsRangedAttacking();
		bool bAIBlocking = AIChar->GetIsBlocking();
		
		if (bPlayerIsThreatening)
		{
			if (bAIBlocking) 
			{
				CurrentGuardTime += DeltaSeconds; // 가드 중이라면 시간 누적
				return;
			}
			// 확률적으로 방어 실행
			if (!AIChar->IsBoosting() && FMath::FRandRange(0.0f, 100.0f) < GuardChance)
			{
				AIChar->StartGuard();
				CurrentGuardTime = 0.0f;
			}
		}
		else
		{
			// 플레이어가 공격을 멈췄을때
			if (bAIBlocking)
			{
				CurrentGuardTime += DeltaSeconds;
				// 현재 가드를 진행한 시간이 최대 보장되는 가드 시간보다 값이 커질 경우
				if (CurrentGuardTime >= MinGuardDuration)
				{
					AIChar->StopGuard(); //가드 해제
					CurrentGuardTime = 0.0f; //가드 타임 0초로 초기화
				}

			}
		}
	}
}

