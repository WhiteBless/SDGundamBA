// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GundamInterfaces.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Execute Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControllingPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!ControllingPawn) return EBTNodeResult::Failed;

	if (ControllingPawn->GetClass()->ImplementsInterface(UGundamCombatInterface::StaticClass()))
	{
		IGundamCombatInterface::Execute_ExecuteAttack(ControllingPawn);
		
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}
