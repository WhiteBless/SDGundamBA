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

// ExiaAICharacter.cpp

void AExiaAICharacter::SetAICombatState(EGundamAICombatState NewState)
{
	AICombatState = NewState;
	
	// 부모에 있는 변수를 강제로 AI 상태와 동기화
	bIsBoosting = (AICombatState == EGundamAICombatState::Boosting);
	bIsFlying = (AICombatState == EGundamAICombatState::Flying);
	
	if (auto* Movement = GetCharacterMovement())
	{
		switch (AICombatState)
		{
		case EGundamAICombatState::Waiting:
			Movement->MaxWalkSpeed = 0.0f;
			break;

		case EGundamAICombatState::Exploring:
			Movement->MaxWalkSpeed = CurrentStat.MoveSpeed * 0.4f;
			Movement->MaxAcceleration = 2048.0f;
			break;

		case EGundamAICombatState::Combat:
			Movement->MaxWalkSpeed = CurrentStat.MoveSpeed;
			Movement->MaxAcceleration = 2048.0f;
			break;

		case EGundamAICombatState::Boosting:
			Movement->MaxWalkSpeed = CurrentStat.BoostSpeedMultiplier; 
			Movement->MaxAcceleration = 20000.0f; 
			break;
			
			
		}
		
		Movement->MaxFlySpeed = Movement->MaxWalkSpeed;
	}
}
