// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_InputBuffer.h"

#include "ExiaCharacterBase.h"

void UANS_InputBuffer::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (auto* Character = Cast<AExiaCharacterBase>(MeshComp->GetOwner()))
	{
		Character->OpenInputBuffer();
	}
}

void UANS_InputBuffer::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (auto* Character = Cast<AExiaCharacterBase>(MeshComp->GetOwner()))
	{
		Character->CloseInputBuffer();
	}
}