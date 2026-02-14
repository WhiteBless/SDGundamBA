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
	float FinalMult = (CurrentStat.BoostSpeedMultiplier <= 0.1f) ? 1.5f : CurrentStat.BoostSpeedMultiplier;
	
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
			Movement->MaxAcceleration = 1048.0f;

			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			break;

		case EGundamAICombatState::Combat:
			Movement->MaxWalkSpeed = CurrentStat.MoveSpeed;
			Movement->MaxAcceleration = 1048.0f;
			break;

		case EGundamAICombatState::Boosting:
			Movement->MaxWalkSpeed = CurrentStat.MoveSpeed * FinalMult;
			Movement->MaxAcceleration = 3000.0f; 
			break;
			
			
		case EGundamAICombatState::Evading:
			bUseControllerRotationYaw = true; 
			GetCharacterMovement()->bOrientRotationToMovement = false;
			break;
		}
		
		Movement->MaxFlySpeed = Movement->MaxWalkSpeed;
	}
}

void AExiaAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetCharacterMovement())
	{
		if (GetCharacterMovement()->IsFlying() || GetCharacterMovement()->IsFalling())
		{
			if (!bIsFlying) 
			{
				bIsFlying = true;
			}
		}
		else if (GetCharacterMovement()->IsWalking())
		{
			if (bIsFlying)
			{
				bIsFlying = false;
			}
		}
	}
}
