// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "ExiaCharacterBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GundamDataStructs.h"
#include "ExiaAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class SDGUNDAMBA_API AExiaAICharacter : public AExiaCharacterBase
{
	GENERATED_BODY()
	
	AExiaAICharacter();
public:
	// AI 컨트롤러가 가져갈 비헤이비어 트리 에셋
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* TreeAsset;

	// 비헤이비어 트리를 반환하는 함수 선언
	UBehaviorTree* GetBehaviorTree() const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI | State")
	EGundamAICombatState AICombatState = EGundamAICombatState::Waiting;
	
	void SetAICombatState(EGundamAICombatState NewState);
	
	virtual void Tick(float DeltaTime) override;
};
