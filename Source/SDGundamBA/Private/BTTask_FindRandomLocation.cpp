#include "BTTask_FindRandomLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!ControlledPawn) return EBTNodeResult::Failed;

	// 1. 네비게이션 시스템 가져오기
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return EBTNodeResult::Failed;

	// 2. 랜덤한 위치 찾기
	FNavLocation RandomLocation;
	// Pawn의 현재 위치를 기준으로 SearchRadius 내의 갈 수 있는 점을 찾음
	if (NavSys->GetRandomReachablePointInRadius(ControlledPawn->GetActorLocation(), SearchRadius, RandomLocation))
	{
		// 3. 찾은 위치를 블랙보드(우리가 설정한 키)에 저장
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}