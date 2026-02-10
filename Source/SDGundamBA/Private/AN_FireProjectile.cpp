// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_FireProjectile.h"
#include "ExiaCharacterBase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


void UAN_FireProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		AExiaCharacterBase* Character = Cast<AExiaCharacterBase>(MeshComp->GetOwner());
		if (Character)
		{
			AActor* TargetActor = nullptr;

			// 타겟이 AI인 경우
			if (AAIController* AIC = Cast<AAIController>(Character->GetController()))
			{
				if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
				{
					TargetActor = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
				}
			}
			// 플레이어인 경우: 락온 시스템이 있다면 가져오거나, 
			// nullptr을 넘겨 정면으로 나가게 함.
			else if (Character->IsPlayerControlled())
			{
				// TODO: 나중에 락온 컴포넌트가 생기면 여기서 타겟을 가져오기
				// TargetActor = Character->GetLockOnTarget(); 
			}

			// 3. 발사 함수 호출!
			Character->FireRangedWeapon(TargetActor);
		}
	}
}
