// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_AttackHitCheck.h"

#include "ExiaCharacterBase.h"

void UANS_AttackHitCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	// 캐릭터를 찾아 판정 박스를 켭니다.
	if (auto* Character = Cast<AExiaCharacterBase>(MeshComp->GetOwner()))
	{
		// [로그 추가] 스위치가 켜지는지 확인
		UE_LOG(LogTemp, Error, TEXT("ANS: Weapon Collision ENABLED!"));
		Character->SetWeaponCollisionEnabled(true);
	}
}

void UANS_AttackHitCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// 캐릭터를 찾아 판정 박스를 끕니다.
	if (auto* Character = Cast<AExiaCharacterBase>(MeshComp->GetOwner()))
	{
		Character->SetWeaponCollisionEnabled(false);
	}
}