// Fill out your copyright notice in the Description page of Project Settings.


#include "GundamAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "ExiaCharacterBase.h" // 적 캐릭터(플레이어) 인식용


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
		SightConfig->PeripheralVisionAngleDegrees = 180.0f; // 시야각 (건담은 센서가 좋으므로 넓게)
		
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
	
	return OnPossess(InPawn);
	/* if (auto* GundamChar = Cast<AExiaCharacterBase>(InPawn))
	{
		if (GundamChar->AIBehaviorTree && GundamChar->AIBehaviorTree->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*GundamChar->AIBehaviorTree->BlackboardAsset);
			BehaviorTreeComp->StartTree(*GundamChar->AIBehaviorTree);
		}
	}
	*/
}

void AGundamAIController::OnTargtDetected(AActor* Actor, FAIStimulus Stimulus)
{
	// 감지된게 플레이어 인가?
	if (auto* EnemyCharacter = Cast<AExiaCharacterBase>(Actor))
	{
		// 시야에 들어왔다면 (Stimulus.WasSuccessfullySensed() == true)
		if (Stimulus.WasSuccessfullySensed())
		{
			// 블랙보드에 타겟 등록 -> 이제 비헤이비어 트리와 EQS가 이 정보를 씀!
			BlackboardComp->SetValueAsObject(Key_TargetActor, EnemyCharacter);
			BlackboardComp->SetValueAsBool(Key_HasLineOfSight, true);
			
			UE_LOG(LogTemp, Warning, TEXT("AI: Target Detected! %s"), *EnemyCharacter->GetName());
		}
		else
		{
			// 시야에서 사라짐
			BlackboardComp->SetValueAsBool(Key_HasLineOfSight, false);
			// 타겟 정보는 바로 지우지 않고, "마지막 위치"로 이동하게 하거나 추격하게 둡니다.
		}
	}
}

