// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_HitWindow.h"
#include "ExiaCharacterBase.h"

void UANS_HitWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	
	if (auto* Character = Cast<AExiaCharacterBase>(MeshComp->GetOwner()))
	{
		Character->bCanPlayHitReaction = true;
	}
}

void UANS_HitWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	
	if (auto* Character = Cast<AExiaCharacterBase>(MeshComp->GetOwner()))
	{
		Character->bCanPlayHitReaction = false;
	}
}
