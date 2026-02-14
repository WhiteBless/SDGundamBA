#include "BTTask_TrackDirectly.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "ExiaAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"

UBTTask_TrackDirectly::UBTTask_TrackDirectly()
{
	bNotifyTick = true;
	NodeName = "Track Directly (Auto Landing)";
    AcceptanceRadius = 150.0f;
}

EBTNodeResult::Type UBTTask_TrackDirectly::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UBTTask_TrackDirectly::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return;
    
	AExiaAICharacter* AIChar = Cast<AExiaAICharacter>(AIC->GetPawn());
    if (!AIChar) return;
    
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(GetSelectedBlackboardKey()));
	
	if (!TargetActor)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	// 거리 및 방향 계산
	FVector MyLoc = AIChar->GetActorLocation();
	FVector TargetLoc = TargetActor->GetActorLocation();
	FVector DirVector = TargetLoc - MyLoc;
	float DistToTarget = DirVector.Size();
	
	// 디버그 용 로그
	FVector Dir = TargetLoc - MyLoc;
	UE_LOG(LogTemp, Error, TEXT("거리: %f, Z차이: %f, 타겟: %s"), Dir.Size(), Dir.Z, *TargetActor->GetName());
	
	// 도착 체크
	if (DistToTarget <= AcceptanceRadius)
	{
		AIChar->GetCharacterMovement()->StopMovementImmediately();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
    // 이동 모드 전환 로직 (과거 코드 방식)
	UCharacterMovementComponent* MoveComp = AIChar->GetCharacterMovement();
    
	// 비행 시작 조건
	if (DirVector.Z > 200.0f) 
	{
		if (MoveComp->MovementMode != MOVE_Flying)
		{
			MoveComp->SetMovementMode(MOVE_Flying);
		}
	}
	// 높이 차이가 1m(100) 이내로 줄어들면 -> 착지 (걷기)
	else if (FMath::Abs(DirVector.Z) < 100.0f)
	{
		if (MoveComp->MovementMode == MOVE_Flying)
		{
			MoveComp->SetMovementMode(MOVE_Walking);
		}
	}
	
    // 입력 주입
    FVector InputDir = DirVector.GetSafeNormal();
	AIChar->AddMovementInput(InputDir, 1.0f);
	
	// 시선 고정
	AIC->SetFocus(TargetActor);
	
    // 디버그
    DrawDebugLine(GetWorld(), MyLoc, TargetLoc, FColor::Red, false, -1.0f, 0, 2.0f);
}