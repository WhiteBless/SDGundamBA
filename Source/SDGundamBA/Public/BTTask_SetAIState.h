#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GundamDataStructs.h"
#include "BTTask_SetAIState.generated.h"

UCLASS()
class SDGUNDAMBA_API UBTTask_SetAIState : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_SetAIState() { NodeName = TEXT("Set AI Combat State"); }

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 에디터에서 선택할 목표 상태
	UPROPERTY(EditAnywhere, Category = "AI")
	EGundamAICombatState TargetState;
};