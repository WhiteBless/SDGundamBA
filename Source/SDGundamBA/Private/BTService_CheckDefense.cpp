// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckDefense.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_CheckDefense::UBTService_CheckDefense()
{
	NodeName = TEXT("Check Player Attack for Defense");
	Interval = 0.2;
	RandomDeviation = 0.05f;
}

void UBTService_CheckDefense::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AIC = OwnerComp.GetAIOwner();
	AExiaCharacterBase* AIChar = Cast<AExiaCharacterBase>(AIC->GetPawn());

	// 블랙보드에서 타겟을 가져오기
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(GetSelectedBlackboardKey()));
	
	if (AIChar && Target)
	{
		AExiaCharacterBase* PlayerChar = Cast<AExiaCharacterBase>(OwnerComp.GetAIOwner());
		
		if (PlayerChar && PlayerChar->bIsAttacking) 
		{
			// 이미 가드 중이 아닌 경우 확률 적으로 가드 시작
			//매 틱마다 확률 계산을 하면 가드를 올렸다 내렸다 할 수 있으므로 조건 추가
			if (!AIChar->IsBoosting() && FMath::RandRange(0.0f, 100.0f) < GuardChance)
			{
				AIChar->StartGuard();
			}
			else
			{
				AIChar->StopGuard();
			}
		}
	}
}

