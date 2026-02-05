// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_CheckNextCombo.h"
#include "ExiaCharacterBase.h"

void UAN_CheckNextCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (AExiaCharacterBase* Character = Cast<AExiaCharacterBase>(MeshComp->GetOwner()))
		{
			Character->CheckNextCombo();
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("UAN_CheckNextCombo 발동 확인"));
}