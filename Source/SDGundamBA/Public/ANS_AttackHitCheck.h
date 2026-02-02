// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_AttackHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class SDGUNDAMBA_API UANS_AttackHitCheck : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	// 애니메이션에서 이 구간이 시작될 때 실행
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	
	// 애니메이션에서 이 구간이 끝날 때 실행
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};