// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TrackDirectly.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"

UBTTask_TrackDirectly::UBTTask_TrackDirectly()
{
	bNotifyTick = true;
	NodeName = "Track Directly (Input)";
}

EBTNodeResult::Type UBTTask_TrackDirectly::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ACharacter* AIChar = Cast<ACharacter>(AIController->GetPawn());
	if (!AIChar) return EBTNodeResult::Failed;
	
	return EBTNodeResult::InProgress;
}

void UBTTask_TrackDirectly::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	ACharacter* AIChar = Cast<ACharacter>(AIController->GetPawn());
    
	// 블랙보드에서 타겟(TargetActor) 가져오기
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(GetSelectedBlackboardKey()));
	
	UE_LOG(LogTemp, Warning, TEXT("태스크 실행 중... 타겟: %s"), 
	TargetActor ? *TargetActor->GetName() : TEXT("없음(NULL)"));
	
	// 타겟이 존재하지 않거나 죽었으면 실패 처리
	if (!AIChar || !TargetActor)
	{
		FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
		return;
	}
	
	// 벡터 계산
	FVector MyLoc = AIChar->GetActorLocation();
	FVector TargetLoc = TargetActor->GetActorLocation();
	FVector DirToTarget = TargetLoc - MyLoc;
	float DistToTarget = DirToTarget.Size();
	
	// 사거리 안에 들어왔으면 성공 종료 (이제 공격하세요!)
	if (DistToTarget <= AcceptanceRadius)
	{
		// 멈추기 위해 입력을 끊음
		AIChar->GetCharacterMovement()->StopMovementImmediately();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	UCharacterMovementComponent* MoveComp = AIChar->GetCharacterMovement();
    
	// 타겟이 나보다 2미터(200) 이상 높이 있으면 -> 비행 모드
	if (DirToTarget.Z > 200.0f) 
	{
		if (MoveComp->MovementMode != MOVE_Flying)
		{
			MoveComp->SetMovementMode(MOVE_Flying);
		}
	}
	// 타겟과 높이가 비슷하거나 낮으면 -> 걷기 모드 (땅으로 착지)
	else if (FMath::Abs(DirToTarget.Z) < 100.0f)
	{
		// 공중에 떠있는 상태라면 다시 걷기로 전환
		if (MoveComp->MovementMode == MOVE_Flying)
		{
			MoveComp->SetMovementMode(MOVE_Walking);
		}
	}
	
	DirToTarget.Normalize(); // 방향만 남기고 정규화
    
	// AI 입력
	AIChar->AddMovementInput(DirToTarget, 1.0f);
	
	// AI의 시선을 플레이어를 향하도록
	AIController->SetFocus(TargetActor);
	
	{
		DrawDebugLine(GetWorld(), MyLoc, TargetLoc, FColor::Red, false, -1.0f, 0, 2.0f);
	}
}
