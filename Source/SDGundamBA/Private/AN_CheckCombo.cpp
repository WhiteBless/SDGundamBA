// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_CheckCombo.h"
#include "ExiaCharacterBase.h"

void UAN_CheckCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		AExiaCharacterBase* Character = Cast<AExiaCharacterBase>(MeshComp->GetOwner());
		if (Character)
		{
			// 캐릭터에게 "저장된 콤보 입력이 있는지 확인해라"고 명령
			Character->CheckComboInput(); 
		}
	}
}