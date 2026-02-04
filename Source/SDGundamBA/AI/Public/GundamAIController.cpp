// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Public/GundamAIController.h"

#include "ExiaAICharacter.h"
#include "ExiaCharacterBase.h" // 적 캐릭터(플레이어) 인식용
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"



const FName AGundamAIController::Key_TargetActor(TEXT("TargetActor"));
const FName AGundamAIController::Key_HasLineOfSight(TEXT("HasLineOfSight"));

AGundamAIController::AGundamAIController()
{

	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	
	if (SightConfig)
	{
		SightConfig->SightRadius = 3000.0f; // 30미터 감지
		SightConfig->LoseSightRadius = 3500.0f; // 35미터 벗어나면 놓침
		SightConfig->PeripheralVisionAngleDegrees = 170.0f; // 시야각
		
		// 감지 대상 설정 (적, 중립, 아군 다 감지할지)
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		// 퍼셉션 컴포넌트에 시각 설정 등록
		PerceptionComp->ConfigureSense(*SightConfig);
		PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
	}
}

void AGundamAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (AExiaAICharacter* AICharacter = Cast<AExiaAICharacter>(InPawn))
	{
		if (UBehaviorTree* BT = AICharacter->GetBehaviorTree())
		{
			UseBlackboard(BT->BlackboardAsset, BlackboardComp);
			
			RunBehaviorTree(BT);
			UE_LOG(LogTemp, Log, TEXT("AI Controller: Behavior Tree Started!"));
		}
	}
	
	if (PerceptionComp)
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AGundamAIController::OnTargetDetected);
	}

}

void AGundamAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	// 1. 블랙보드 컴포넌트를 가져옵니다.
	UBlackboardComponent* BlackboardPtr = GetBlackboardComponent();

	// 혹시 모르니 블랙보드가 진짜 있는지 체크!
	if (BlackboardPtr)
	{
		// 적을 성공적으로 감지했는가? (Yes)
		if (Stimulus.WasSuccessfullySensed())
		{
			// 블랙보드의 "TargetActor"라는 키(Key)에 감지한 대상을 저장!
			BlackboardPtr->SetValueAsObject(TEXT("TargetActor"), Actor);
            
			UE_LOG(LogTemp, Warning, TEXT("적 발견! 추적을 시작합니다: %s"), *Actor->GetName());
		}
		// 적을 놓쳤는가? (No)
		else
		{
			// 블랙보드를 비워서 추적을 멈추게 함
			BlackboardPtr->SetValueAsObject(TEXT("TargetActor"), nullptr);
            
			UE_LOG(LogTemp, Warning, TEXT("적을 놓쳤습니다."));
		}
	}
}

FGenericTeamId AGundamAIController::GetGenericTeamId() const
{
	// 1번 팀(적군)이라고 알려줌
	return FGenericTeamId(1);
}