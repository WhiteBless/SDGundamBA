// Fill out your copyright notice in the Description page of Project Settings.


#include "ExiaAICharacter.h"
#include "AI/Public/GundamAIController.h"

AExiaAICharacter::AExiaAICharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	AIControllerClass = AGundamAIController::StaticClass();
	bForceBufferInput = true;
}

UBehaviorTree* AExiaAICharacter::GetBehaviorTree() const
{
    return TreeAsset;
}

