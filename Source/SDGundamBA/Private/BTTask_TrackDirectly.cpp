#include "BTTask_TrackDirectly.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GundamDataStructs.h"
#include "ExiaAICharacter.h"
#include "DrawDebugHelpers.h"

UBTTask_TrackDirectly::UBTTask_TrackDirectly()
{
    bNotifyTick = true;
    NodeName = "Track Directly";
}

EBTNodeResult::Type UBTTask_TrackDirectly::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    return EBTNodeResult::InProgress;
}

void UBTTask_TrackDirectly::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIC = OwnerComp.GetAIOwner();
    AExiaAICharacter* AIChar = Cast<AExiaAICharacter>(AIC->GetPawn());
    
    // 타겟 가져오기
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(GetSelectedBlackboardKey()));

    if (!AIChar || !TargetActor)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    bool bTargetInAir = Blackboard->GetValueAsBool(FName("bIsTargetInAir"));

    UCharacterMovementComponent* MoveComp = AIChar->GetCharacterMovement();
    FVector MyLoc = AIChar->GetActorLocation();
    FVector TargetLoc = TargetActor->GetActorLocation();
    
    // 방향 벡터 계산 (3D)
    FVector DirToTarget = TargetLoc - MyLoc;
    float DistToTarget = DirToTarget.Size();
    FVector LookDir = DirToTarget.GetSafeNormal(); // 정규화된 방향 벡터
    
    // 타겟이 나보다 150cm 이상 위에 있거나, 이미 비행 중이라면 비행 모드 유지
    bool bShouldFly = (DirToTarget.Z > 150.0f) || MoveComp->IsFlying();
    
    // 타겟이 바닥에 있고(공중 아님) 나도 바닥에 가까우면 걷기 모드
    if (FMath::Abs(DirToTarget.Z) < 50.0f && !bTargetInAir) 
    {
        bShouldFly = false;
    }

    // 이동 모드 전환 (매 프레임 체크하되, 조건 만족 시 확실하게 전환)
    if (bShouldFly)
    {
        if (MoveComp->MovementMode != MOVE_Flying)
        {
            MoveComp->SetMovementMode(MOVE_Flying);
        }
    }
    else
    {
        if (MoveComp->MovementMode != MOVE_Walking && MoveComp->MovementMode != MOVE_Falling)
        {
            MoveComp->SetMovementMode(MOVE_Walking);
        }
    }
    
    // Flying 모드일 때는 Z축이 포함된 LookDir를 그대로 넣어야 위로 올라갑니다.
    // Walking 모드일 때는 어차피 Z축이 무시되므로 동일하게 넣어도 됩니다.
    AIChar->AddMovementInput(LookDir, 1.0f);

    // 회전 처리 (부드럽게 타겟 바라보기)
    FRotator TargetRot = LookDir.Rotation();
    FRotator CurrentRot = AIChar->GetActorRotation();
    
    // RInterpTo를 사용하여 부드럽게 회전 (속도 10.0f 조절 가능)
    FRotator SmoothRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaSeconds, 10.0f);
    
    // 비행 중일 때는 위아래(Pitch)도 회전해야 자연스러움
    if (bShouldFly)
    {
        AIC->SetControlRotation(SmoothRot); // 컨트롤러 회전 동기화
        AIChar->SetActorRotation(SmoothRot); // 액터 회전 적용
    }
    else
    {
        FRotator GroundRot = FRotator(0, SmoothRot.Yaw, 0);
        AIChar->SetActorRotation(GroundRot);
        AIC->SetFocus(TargetActor);
        
    }

    // 거리 체크 및 종료
    if (DistToTarget <= AcceptanceRadius)
    {
        MoveComp->StopMovementImmediately();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}