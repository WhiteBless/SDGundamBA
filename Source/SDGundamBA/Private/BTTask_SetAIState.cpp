#include "BTTask_SetAIState.h"
#include "AIController.h"
#include "ExiaAICharacter.h"

EBTNodeResult::Type UBTTask_SetAIState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AExiaAICharacter* AIChar = Cast<AExiaAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		AIChar->SetAICombatState(TargetState);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}