// Fill out your copyright notice in the Description page of Project Settings.

#include "ExiaAnimInstance.h"
#include "ExiaCharacterBase.h" // AI 캐릭터도 이 클래스를 상속받아야 합니다!
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UExiaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    auto Pawn = TryGetPawnOwner();
    AExiaCharacterBase* Character = Cast<AExiaCharacterBase>(Pawn);
    
    if (Character == nullptr) return;
    if (!IsValid(Pawn)) return;
    
    if (auto* Movement = Character->GetCharacterMovement())
    {
        GroundSpeed = Character->GetVelocity().Size2D();
        bIsFalling = Movement->IsFalling();
        
        bIsFlying = (Movement->MovementMode == EMovementMode::MOVE_Flying);
    }
    
    if (Character)
    {
        // 기본 상태 업데이트
        bIsBoosting = Character->IsBoosting();
        bIsJumping = Character->bIsJumping;
        bCanJump = Character->bCanJump;
        
        // 무브먼트 컴포넌트 데이터 갱신
        if (auto* Movement = Character->GetCharacterMovement())
        {
            bIsFalling = Movement->IsFalling();
            bIsAscending = Pawn->GetVelocity().Z > 10.f;
        }
        
        // AI가 플라잉 상태인지를 저장
        // TODO 허공 걷기 모션 해결책
        if (auto* Movement = Character->GetCharacterMovement())
        {
            bIsFalling = Movement->IsFalling();
            bIsFlying = (Movement->MovementMode == EMovementMode::MOVE_Flying);
        }
        
        // 이동 벡터 계산 (AI와 플레이어 공용 로직)
        // 플레이어: 키보드 입력(InputVector) 사용
        FVector InputVector = Character->GetLastMovementInputVector(); 
        FVector CurrentVelocity = Pawn->GetVelocity();
        
        // AI는 키보드 입력이 없으므로(InputVector가 0), 
        // 실제로 움직이고 있다면(Velocity가 있다면) 속도 방향을 입력값으로 간주합니다.
        if (InputVector.IsNearlyZero() && CurrentVelocity.SizeSquared2D() > 1.0f)
        {
            InputVector = CurrentVelocity.GetSafeNormal2D();
        }

        // 4. 로컬 방향으로 변환 (월드 좌표 -> 캐릭터 기준 좌표)
        FRotator CameraRot = Pawn->GetControlRotation();
        
        // AI 컨트롤러는 ControlRotation이 없을 수도 있으므로, 액터 회전값을 예비로 사용
        if (Pawn->IsPlayerControlled() == false)
        {
            CameraRot = Pawn->GetActorRotation();
        }

        FRotator YawRotation(0.0f, CameraRot.Yaw, 0.0f);
        
        // 회전을 풀어 로컬 방향(앞/뒤/좌/우)을 구함
        FVector LocalInput = YawRotation.UnrotateVector(InputVector).GetSafeNormal();

        // 5. 애니메이션 블렌딩 목표 속도 설정
        // 부스팅 중이면 180, 걷기면 90 (기존 로직 유지)
        float TargetSpeed = bIsBoosting ? 180.0f : 90.0f;
        
        // 입력도 없고 실제 속도도 없으면 멈춤 처리
        if (InputVector.IsNearlyZero())
        {
            TargetSpeed = 0.0f;
        }

        // 6. 목표값 계산 (X: 앞뒤, Y: 좌우)
        float TargetForward = LocalInput.X * TargetSpeed;
        float TargetRight = LocalInput.Y * TargetSpeed;

        // 7. 부드러운 보간 (Interpolation)
        LocalVelocityForward = FMath::FInterpTo(LocalVelocityForward, TargetForward, DeltaSeconds, 10.0f);
        LocalVelocityRight = FMath::FInterpTo(LocalVelocityRight, TargetRight, DeltaSeconds, 10.0f);

        // 8. 대시 방향 기억 (잔상 효과 등을 위해)
        if (bIsBoosting && InputVector.Size() >= 0.2f)
        {
            LastDashForward = LocalVelocityForward;
            LastDashRight = LocalVelocityRight;
        }

        // 디버그 (개발용)
        /*
        if (GEngine)
        {
            FString DebugMsg = FString::Printf(TEXT("Fwd: %.2f / Right: %.2f (Boost: %d)"), 
                LocalVelocityForward, LocalVelocityRight, bIsBoosting);
            GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Cyan, DebugMsg);
        }
        */
    }
}
