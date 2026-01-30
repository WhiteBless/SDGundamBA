// Fill out your copyright notice in the Description page of Project Settings.


#include "ExiaAnimInstance.h"
#include "ExiaCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UExiaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    auto Pawn = TryGetPawnOwner();
    if (!IsValid(Pawn)) return;

    AExiaCharacterBase* Character = Cast<AExiaCharacterBase>(Pawn);
    if (Character)
    {
        // 기본 상태 업데이트
        bIsBoosting = Character->IsBoosting();
        bIsJumping = Character->bIsJumping;
        bCanJump = Character->bCanJump;
        
        if (auto* Movement = Character->GetCharacterMovement())
        {
            bIsFalling = Movement->IsFalling();
            bIsAscending = Pawn->GetVelocity().Z > 10.f;
        }

        // 이동 방향 계산 (카메라 Yaw 기준)
        FVector InputVector = Character->GetLastMovementInputVector(); 
        
        FRotator CameraRot = Pawn->GetControlRotation();
        FRotator YawRotation(0.0f, CameraRot.Yaw, 0.0f);

        // ★ [핵심 수정] .GetSafeNormal()을 붙여서 대각선 입력(1.414)을 1.0으로 만듭니다.
        // 이렇게 해야 블렌드 스페이스가 헷갈려하지 않고 부드럽게 섞입니다.
        FVector LocalInput = YawRotation.UnrotateVector(InputVector).GetSafeNormal();

        // 3. 속도 결정
        float TargetSpeed = bIsBoosting ? 180.0f : 90.0f;
        
        if (InputVector.IsNearlyZero())
        {
            TargetSpeed = 0.0f;
        }

        // 목표 속도 계산
        float TargetForward = LocalInput.X * TargetSpeed;
        float TargetRight = LocalInput.Y * TargetSpeed;

        // 보간 (Interpolation)
        LocalVelocityForward = FMath::FInterpTo(LocalVelocityForward, TargetForward, DeltaSeconds, 10.0f);
        LocalVelocityRight = FMath::FInterpTo(LocalVelocityRight, TargetRight, DeltaSeconds, 10.0f);

        // 대시 방향 기억
        if (bIsBoosting && InputVector.Size() >= 0.2f)
        {
            LastDashForward = LocalVelocityForward;
            LastDashRight = LocalVelocityRight;
        }

        // 디버그
        if (GEngine)
        {
            FString DebugMsg = FString::Printf(TEXT("InX: %.2f / InY: %.2f (Size: %.2f)"), 
                LocalInput.X, LocalInput.Y, LocalInput.Size());
            GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Green, DebugMsg);
        }
    }
}