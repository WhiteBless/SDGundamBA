// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_CheckCombo.generated.h"

/**
 * 
 */
UCLASS()
class SDGUNDAMBA_API UAN_CheckCombo : public UAnimNotify
{
	GENERATED_BODY()

public:
	// 노티파이가 실행될 때 호출
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};