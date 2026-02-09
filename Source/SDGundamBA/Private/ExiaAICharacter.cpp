// Fill out your copyright notice in the Description page of Project Settings.


#include "ExiaAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
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

void AExiaAICharacter::SetAICombatState(EGundamAICombatState NewState)
{
	AICombatState = NewState;
    
	if (auto* Movement = GetCharacterMovement())
	{
		switch (AICombatState)
		{
		case EGundamAICombatState::Waiting:
			Movement->MaxWalkSpeed = 0.0f; // 대기 시 멈춤
			break;
		case EGundamAICombatState::Exploring:
			Movement->MaxWalkSpeed = CurrentStat.MoveSpeed * 0.4f; // 정찰 시 느리게 걷기
			break;
		case EGundamAICombatState::Combat:
			Movement->MaxWalkSpeed = CurrentStat.MoveSpeed; // 전투 시 전속력
			break;
		}
	}
}

