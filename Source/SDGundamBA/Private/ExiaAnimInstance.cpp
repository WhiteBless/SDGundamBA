// Fill out your copyright notice in the Description page of Project Settings.


#include "ExiaAnimInstance.h"
#include "ExiaCharacterBase.h"

void UExiaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (IsValid(Pawn))
	{
		// 캐릭터의 부스트 상태 가져오기
		AExiaCharacterBase* Character = Cast<AExiaCharacterBase>(Pawn);
		if (Character)
		{
			FVector InputVector = Character->GetLastMovementInputVector();
			bIsBoosting = Character->IsBoosting();
			
			// 월드 속도를 로컬 속도로 변환
			FVector Velocity = Pawn->GetVelocity();
			FRotator Rotation = Pawn->GetActorRotation();
        
			// [핵심] 입력 방향을 로컬 좌표로 변환 (S를 누르면 즉시 X가 -1이 됨)
			FVector LocalInput = Rotation.UnrotateVector(InputVector);
		
	
			// 변수에 저장
			float DashSpeed = 180.0f;
			float TargetForward = LocalInput.X * DashSpeed;
			float TargetRight = LocalInput.Y * DashSpeed;
			LocalVelocityForward = FMath::FInterpTo(LocalVelocityForward, TargetForward, DeltaSeconds, 10.0f);
			LocalVelocityRight = FMath::FInterpTo(LocalVelocityRight, TargetRight, DeltaSeconds, 10.0f);
		}
		
		if (bIsBoosting)
		{
			FVector CurrentInput = Character->GetLastMovementInputVector();
			if (CurrentInput.Size() >= 0.2f)
			{
				LastDashForward = LocalVelocityForward;
				LastDashRight = LocalVelocityRight;
			}
		}
	}
}
