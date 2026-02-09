// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TrackDirectly.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GundamDataStructs.h"
#include "ExiaAICharacter.h"
#include "DrawDebugHelpers.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"

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
	AExiaAICharacter* AIChar = Cast<AExiaAICharacter>(AIController->GetPawn());
    
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
	UCharacterMovementComponent* MoveComp = AIChar->GetCharacterMovement();

	bool bTargetInAir = OwnerComp.GetBlackboardComponent()->GetValueAsBool(FName("bIsTargetInAir"));
	
	if (bTargetInAir)
	{
		if (MoveComp->MovementMode == MOVE_Flying)
		{
			// 공중일 때는 기존처럼 직선 추격
			MoveComp->SetMovementMode(MOVE_Flying);
		}
		
		FVector Dir = TargetLoc - MyLoc;
		AIChar->AddMovementInput(Dir.GetSafeNormal(), 1.0f);
	}

	else
	{
		if (MoveComp->MovementMode == MOVE_Flying && !MoveComp->IsFalling())
		{
			MoveComp->SetMovementMode(MOVE_Walking);
		}
		// 지상일 때는 네비게이션 시스템에 "길"을 물어봅니다.
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
		UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(GetWorld(), MyLoc, TargetLoc);

		// 갈 수 있는 길(PathPoints)이 있고 장애물이 있다면
		if (NavPath && NavPath->PathPoints.Num() > 1)
		{
			// 바로 다음 길목(Index 1)을 향해 방향 벡터를 설정합니다.
			DirToTarget = NavPath->PathPoints[1] - MyLoc;
		}
		else
		{
			// 길이 없으면 직선으로 시도
			DirToTarget = TargetLoc - MyLoc;
		}
	}
	
	FVector FinalDir = DirToTarget.GetSafeNormal(); 
	AIChar->AddMovementInput(FinalDir, 1.0f);
	
	//TODO 개선해야 할것.
	// 타겟이 나보다 높이 있으면 -> 비행 모드 전환
	// 플레이어의 상태를 읽어와서 전환할 필요가 있음
	// 높이 값으로 하니 정밀하지 않음.
	// 내가 부스트 상태면 동일하게 부스트 상태로 전환해 플레이어를 추적해야하며
	// 점프 또는 공중 상태일때는 동일한 모션 모드로 전환할 필요가 있음.
	if (FMath::Abs(DirToTarget.Z) > 200.0f) 
	{
		if (MoveComp->MovementMode != MOVE_Flying)
		{
			MoveComp->SetMovementMode(MOVE_Flying);
		}
	}
	
	else if (FMath::Abs(DirToTarget.Z) < 100.0f && MoveComp->MovementMode == MOVE_Flying)
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}
	
	if (DistToTarget > 1000.0f)
	{
		// 거리가 멀면 부스트 상태로 전환하여 빠르게 접근
		AIChar->SetAICombatState(EGundamAICombatState::Boosting);
	}
	
	// 사거리 안에 들어왔으면 종료
	if (DistToTarget <= AcceptanceRadius)
	{
		MoveComp->StopMovementImmediately();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	FVector MoveDir = DirToTarget.GetSafeNormal();
	AIChar->AddMovementInput(MoveDir, 1.0f);

	DirToTarget.Normalize(); // 방향만 남기고 정규화
    
	// AI 입력
	AIChar->AddMovementInput(DirToTarget, 1.0f);
	
	// AI의 시선을 플레이어를 향하도록
	AIController->SetFocus(TargetActor);
	
	{
		DrawDebugLine(GetWorld(), MyLoc, TargetLoc, FColor::Red, false, -1.0f, 0, 2.0f);
	}
}
