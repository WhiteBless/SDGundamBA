// Fill out your copyright notice in the Description page of Project Settings.



// #include "GameFramework/SpringArmComponent.h"
// #include "Camera/CameraComponent.h"
// #include "EnhancedInputComponent.h"
// #include "EnhancedInputSubsystems.h"

#include "ExiaCharacterBase.h"

#include "BrainComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "InputAction.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Engine/DataTable.h"
#include "SDGundamBA.h"
#include "AI/Public/GundamAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AExiaCharacterBase::AExiaCharacterBase()
{
	bCanGuard = true;
	GuardCooldownTime = 2.0f;
	
	PrimaryActorTick.bCanEverTick = true;
	
	// SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// SpringArmComp->SetupAttachment(RootComponent);
	// SpringArmComp->TargetArmLength = 500.0f;
	// SpringArmComp->bUsePawnControlRotation = true;
	
	//무기 콜리전
	WeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollision"));
	WeaponCollision->SetupAttachment(GetMesh(), FName("Forearm_WeaponSocket_R"));
	WeaponCollision->SetCollisionProfileName(TEXT("GN_Sword"));

	// CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// CameraComp->SetupAttachment(SpringArmComp);
	
	MotionWarpingComp = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));

	GuardShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GuardShieldMesh"));
	GuardShieldMesh->SetupAttachment(GetMesh()); // 캐릭터 메쉬에 부착 (소켓 연결 추천)
	GuardShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌은 없게 (판정은 bBlock변수로 하니까)
	GuardShieldMesh->SetHiddenInGame(true); // 게임 시작 시에는 숨김
	
	JumpMaxHoldTime = 0.0f;
	
	CurrentGNParticles = 200.0f;
	
	GetCharacterMovement()->JumpZVelocity = 900.f;
	
	// 가드 콜리전
	GuardCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("GuardCollision"));
	GuardCollision->SetupAttachment(GetMesh(), FName("ShieldSocket")); // 방패나 팔 소켓에 부착
	GuardCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 기본은 꺼둠
	GuardCollision->SetCollisionProfileName(TEXT("BlockAllDynamic")); // 투사체와 충돌 가능하도록 설정
}

void AExiaCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	LoadCharacterData();
	
	//가드 콜리전
	if (GuardCollision)
	{
		GuardCollision->OnComponentBeginOverlap.AddDynamic(this, &AExiaCharacterBase::OnGuardOverlap);
	}
	
	//무기 콜리전
	if (WeaponCollision)
	{
		WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AExiaCharacterBase::OnWeaponOverlap);
	}

	// if (APlayerController* PC = Cast<APlayerController>(GetController()))
	// {
	// 	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	// 	{
	// 		if (DefaultMappingContext)
	// 		{
	// 			Subsystem->AddMappingContext(DefaultMappingContext, 0);
	// 		}
	// 		else
	// 		{
	// 			UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext가 할당되지 않았습니다!"));
	// 		}
	// 	}
	// }
}

void AExiaCharacterBase::OpenInputBuffer()
{
	bIsBufferWindowOpen = true;
	bHasBufferedInput = false; // 창이 열릴 때 초기화
}

void AExiaCharacterBase::CloseInputBuffer()
{
	bIsBufferWindowOpen = false;
}

void AExiaCharacterBase::ResetGuardCooldown()
{
	bCanGuard = true;
	GetWorld()->GetTimerManager().ClearTimer(GuardCooldownTimer);
	UE_LOG(LogTemp, Warning, TEXT("Guard Ready! (Cooldown Finished)"));
}

void AExiaCharacterBase::StartGuard()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	// 공중 상태라면 가드 불가
	if (GetCharacterMovement()->IsFalling()) return;
	
	if (!AnimInstance || bIsStunned || !bCanGuard || bBlock) return;

	if (AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		return; 
	}
	
	if (bIsAttacking && !AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		bIsAttacking = false;
	}
	
	bBlock = true;
	
	if (GuardShieldMesh) GuardShieldMesh->SetHiddenInGame(false);
	if (GuardCollision) GuardCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCharacterMovement()->MaxWalkSpeed = CurrentStat.MoveSpeed * 0.85f;
	
	// 이펙트(GN 필드) 보이게 설정 (Hidden = false)
	if (GuardShieldMesh)
	{
		GuardShieldMesh->SetHiddenInGame(false);
	}
	
	if (GuardMontage)
	{
		// 몽타주 재생
		PlayAnimMontage(GuardMontage, 1.0f, GuardLoopSectionName);
	}

	UE_LOG(LogTemp, Log, TEXT("가드 활성화 상태."));
}

void AExiaCharacterBase::StopGuard()
{
	// 이미 가드가 풀려있다면 패스
	if (!bBlock) return;
	
	// 가드 상태 해제
	bBlock = false;
	
	if (GuardShieldMesh) GuardShieldMesh->SetHiddenInGame(true);
	GetCharacterMovement()->MaxWalkSpeed = CurrentStat.MoveSpeed;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && GuardMontage)
	{
		// 0.2초 여유를 두고 부드럽게 기본 포즈로 돌아갑니다.
		AnimInstance->Montage_Stop(0.2f, GuardMontage);
	}
	
	// 이펙트 숨기기 (Hidden = true)
	if (GuardShieldMesh) GuardShieldMesh->SetHiddenInGame(true);
	if (GuardCollision) GuardCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// 이동 속도 복구
	GetCharacterMovement()->MaxWalkSpeed = CurrentStat.MoveSpeed;

	// 쿨타임 타이머
	bCanGuard = false;
	GetWorld()->GetTimerManager().SetTimer(GuardCooldownTimer, this, &AExiaCharacterBase::ResetGuardCooldown, GuardCooldownTime, false);
}

void AExiaCharacterBase::ExecuteAttack_Implementation()
{
    // 상태 체크
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance || bIsStunned) return;

    UE_LOG(LogTemp, Log, TEXT("=== ExecuteAttack 호출됨 (현재 콤보: %d) ==="), AttackComboCount);

	if (bBlock)
	{
		bBlock = false;;
		if (AnimInstance && GuardMontage) AnimInstance->Montage_Stop(0.1f, GuardMontage);
        
		if (GuardShieldMesh) GuardShieldMesh->SetHiddenInGame(true);
		if (GuardCollision) GuardCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        
		GetCharacterMovement()->MaxWalkSpeed = CurrentStat.MoveSpeed;
		
		bCanGuard = true;
	}

    // AI 포커스 또는 Sweep으로 타겟 찾기
    AActor* BestTarget = nullptr;

    // AI인 경우 컨트롤러가 보고 있는 대상을 우선 타겟으로 설정
    if (!IsPlayerControlled())
    {
        if (AAIController* AIC = Cast<AAIController>(GetController()))
        {
            BestTarget = AIC->GetFocusActor();
        }
    }

    // 타겟이 없거나 플레이어인 경우 기존 Sweep(Sphere Trace)으로 찾기
    if (!BestTarget)
    {
        FVector Start = GetActorLocation();
        FVector End = Start + (GetActorForwardVector() * 2500.0f);
        float Radius = 300.0f;

        FCollisionObjectQueryParams ObjectQueryParams;
        ObjectQueryParams.AddObjectTypesToQuery(GundamCollision::BossEnemy);
        ObjectQueryParams.AddObjectTypesToQuery(GundamCollision::GundamPlayer);

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        bool bHit = GetWorld()->SweepSingleByObjectType(HitResult, Start, End, FQuat::Identity, ObjectQueryParams, FCollisionShape::MakeSphere(Radius), Params);
        
        if (bHit) BestTarget = HitResult.GetActor();

        // [디버그용] 탐색 범위 표시
        DrawDebugCapsule(GetWorld(), Start + (GetActorForwardVector() * 250.0f), 150.0f, Radius, FQuat::Identity, FColor::Green, false, 1.0f);
    }

    // 3. 모션 워핑 타겟 설정 (찾은 타겟이 있다면)
    if (BestTarget)
    {
        SetWarpTarget(BestTarget);
        DrawDebugSphere(GetWorld(), BestTarget->GetActorLocation(), 30.0f, 12, FColor::Red, false, 2.0f);
    }
    else
    {
        SetWarpTarget(nullptr);
    }

    // 4. 공격 실행 및 선입력(Combo Buffer) 처리
    if (AnimInstance->Montage_IsPlaying(AttackMontage))
    {
        // 공격 중이라면 다음 공격 예약
        if (bIsBufferWindowOpen || bForceBufferInput) 
        {
            bHasBufferedInput = true;
            UE_LOG(LogTemp, Warning, TEXT("Combo Buffered!"));
        }
        return; 
    }

    // 5. 실제 첫 공격 시작
    bIsAttacking = true; 
    bHasBufferedInput = false;
    AttackComboCount = 0;
    
    // 콤보 명령 처리 함수 호출 (실제 몽타주 재생 로직이 들어있는 함수)
    ProcessComboCommand();
}

void AExiaCharacterBase::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage)
	{
		bIsAttacking = false;
		AttackComboCount = 0;
		UE_LOG(LogTemp, Log, TEXT("Attack State Cleared."));
	}
}

void AExiaCharacterBase::ResettingComboAttack()
{
	AttackComboCount = 0; // 
	bIsAttacking = false; // [cite: 16]
}

void AExiaCharacterBase::OnWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
										 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
										 bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this || HitActors.Contains(OtherActor)) return;

	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UGundamCombatInterface::StaticClass()))
	{
		// [핵심] 칼날 박스 위치를 기본값으로 하되, 적 콜리전과 가장 가까운 접점을 찾습니다.
		FVector HitPoint = WeaponCollision->GetComponentLocation();
		if (OtherComp)
		{
			OtherComp->GetClosestPointOnCollision(WeaponCollision->GetComponentLocation(), HitPoint);
		}

		// 수정한 인터페이스 호출 (HitPoint를 마지막 인자로 전달)
		IGundamCombatInterface::Execute_ApplyGundamDamage(OtherActor, 10.0f, this, FName("Body"), HitPoint);
        
		HitActors.Add(OtherActor);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Target %s does not have GundamCombatInterface!"), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));
	}
}

void AExiaCharacterBase::OnGuardBreak()
{
	bBlock = false;
	bIsStunned = true;
	
	if (GuardMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// 가드 중이던 몽타주에서 바로 경직 섹션으로 점프합니다.
			AnimInstance->Montage_JumpToSection(FName("GuardBreak"), GuardMontage);
		}
	}
	
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* BrainComp = AIC->GetBrainComponent())
		{
			// "Stunned"라는 이유로 로직을 일시 정지시킵니다.
			BrainComp->PauseLogic(TEXT("Stunned")); 
		}
	}
	
	UE_LOG(LogTemp, Error, TEXT("!!! GUARD BREAK !!! Stunned for %f seconds"), StunDuration);

	// 일정 시간 뒤 경직 해제 타이머 설정
	FTimerHandle StunTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, this, &AExiaCharacterBase::RecoverFromStun, StunDuration, false);
}

void AExiaCharacterBase::RecoverFromStun()
{
	bIsStunned = false;
	
	CurrentStat.CurrentGuardHP = CurrentStat.MaxGuardHP * 0.3f;
	
	// 스턴 해재 AI로직 재개
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* BrainComp = AIC->GetBrainComponent())
		{
			BrainComp->ResumeLogic(TEXT("Stunned Recovery"));
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("AI Recovered from Stun. Logic Resumed."));
}

void AExiaCharacterBase::RegenerateGuardHP(float DeltaTime)
{
}

void AExiaCharacterBase::ApplyGundamDamage_Implementation(float DamageAmount, AActor* Attacker, FName HitBoneName, FVector HitLocation)
{
	UE_LOG(LogTemp, Error, TEXT("DAMAGE RECEIVED! Amount: %f"), DamageAmount);
	
	if (CurrentHP <= 0.0f) return;

	// 방어력을 적용한 실제 데미지 계산
	float ActualDamage = FMath::Max(DamageAmount - DefensePower, 1.0f);

	// 피격 상태 판별 (스턴 > 가드 > 일반 피격 순서)
    
	// 경직(Stun) 상태: 가드 불가, 100% 데미지
	if (bIsStunned)
	{
		CurrentHP = FMath::Clamp(CurrentHP - DamageAmount, 0.0f, MaxHP);
		UE_LOG(LogTemp, Warning, TEXT("Stunned Hit! Damage: %f"), DamageAmount);
	}
	// 가드(Block) 상태: 체력 대신 가드 게이지 차감
	else if (bBlock)
	{
		CurrentGuardHP -= DamageAmount; // 가드는 방어력 적용 전 수치로 차감
		UE_LOG(LogTemp, Warning, TEXT("Blocked! Guard HP: %f / %f"), CurrentGuardHP, MaxGuardHP);

		if (CurrentGuardHP <= 0.0f)
		{
			CurrentGuardHP = 0.0f;
			OnGuardBreak(); // 가드 브레이크 발생
		}
        
		// 가드 성공 시 함수 종료 (체력 차감 방지)
		return; 
	}
	// 일반 피격 상태
	else 
	{
		CurrentHP = FMath::Clamp(CurrentHP - ActualDamage, 0.0f, MaxHP);
		UE_LOG(LogTemp, Warning, TEXT("Hit! Damage: %f, CurrentHP: %f"), ActualDamage, CurrentHP);
	}

	// 3. 사망 판정
	if (CurrentHP <= 0.0f)
	{
		CurrentHP = 0.0f;
		UE_LOG(LogTemp, Error, TEXT("%s Destroyed!"), *GetName());
		// OnDeath(); // 사망 로직 호출
		return;
	}

	// 피격 애니메이션 및 방향 처리
	if (HitMontage && Attacker)
	{
		// 슈퍼아머 상태라면 애니메이션 재생 건너뛰기 로직 추가 가능
        
		FVector ToAttacker = Attacker->GetActorLocation() - GetActorLocation();
		ToAttacker.Normalize();
        
		float ForwardDot = FVector::DotProduct(GetActorForwardVector(), ToAttacker);
        
		// 정면/후면 섹션 결정 (ForwardDot > 0 이면 정면)
		FName SectionName = (ForwardDot >= 0.5f) ? FName("Hit_Front") : FName("Hit_Back");

		PlayAnimMontage(HitMontage, 1.0f, SectionName);
	}
}

void AExiaCharacterBase::OnGuardOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		// 1. 부딪힌 액터가 투사체(미사일 등)인지 확인 (인터페이스나 클래스 체크)
		// 2. 투사체라면 폭발 이펙트를 이 위치(SweepResult.ImpactPoint)에서 재생
		// 3. 본체에 전달될 대미지를 가드 대미지로 변환하여 적용
		UE_LOG(LogTemp, Warning, TEXT("Projectile Blocked by Shield Collision!"));	
	}
	
}

void AExiaCharacterBase::BlockingStateStart()
{
	if (bBlock)
	{
		GetCharacterMovement()->GravityScale = 10.5f;
		GetCharacterMovement()->MaxWalkSpeed = CurrentStat.MoveSpeed * 0.8f; // 이동속도 20% 감소
	}
}

void AExiaCharacterBase::BlockingStateEnd()
{
	if (!bBlock)
	{
		GetCharacterMovement()->GravityScale = DefaultGravityScale;
		GetCharacterMovement()->MaxWalkSpeed = CurrentStat.MoveSpeed;
	}
}

void AExiaCharacterBase::LoadCharacterData()
{
	if (StatDataTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterDataTable is missing!"));
		return;
	}
	
	if (StatDataTable)
	{
		static const FString ContextString(TEXT("Character Data Context"));
		FGundamCharacterData* CharData = StatDataTable->FindRow<FGundamCharacterData>(CharacterRowName, ContextString);
		// 데이터를 찾았다면 변수에 할당
		if (CharData)
		{
			CurrentStat = *CharData;
			CurrentStat.CurrentGuardHP = CurrentStat.MaxGuardHP; // LoadCharcterDate를 불러올때 현재 가드HP를 MaxGuardHP에 맞춰 풀 충전
			
			auto* Movement = GetCharacterMovement();
			if (Movement)
			{
				Movement->MaxWalkSpeed = CurrentStat.MoveSpeed;
				Movement->JumpZVelocity = CurrentStat.JumpZVelocity;
				Movement->GravityScale = CurrentStat.DefaultGravityScale;
			}
			// 최대 체력을 데이터 테이블 값으로 설정
			MaxHP = CharData->MaxHP;
			
			// 게임 시작시 체력 회복
			CurrentHP = CurrentStat.MaxHP;
			CurrentGNParticles = CurrentStat.GNParticles;

			// 이동 속도나 부스트 양도 여기서 같이 세팅
			GetCharacterMovement()->MaxWalkSpeed = CharData->MoveSpeed;
			
			UE_LOG(LogTemp, Log, TEXT("Data Loaded! MaxHP: %f"), MaxHP);
			UE_LOG(LogTemp, Warning, TEXT("Row [%s] Loaded! Actual MaxHP: %f"), *CharacterRowName.ToString(), MaxHP);
		}
	}
}

void AExiaCharacterBase::StartFlying()
{
	if (bBlock) return;
	
	bIsFlying = true;
	
	if (GetCharacterMovement())
	{
		// 비행 시작
		GetCharacterMovement()->GravityScale = FlightGravityScale;
		
		// 공중에서 즉시 멈칫하는 현상을 방지하기 위해 속도 제동 보정
		//GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	}
}

void AExiaCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	bIsJumping = false;
	bHasJumped = false;
	bHasJumpDashUsed = false;
	bCanJump = false;
	
	if (bIsJumpBoosting)
	{
		StopJumpDash();
	}
	
	if (bIsFlying)
	{
		StopFlying(); 
	}
	
	if (bIsBoosting)
	{
		StopBoost();
	}
	
	GetCharacterMovement()->DisableMovement(); // 잠시 멈춤
	GetWorldTimerManager().SetTimer(LandingTimerHandle, this, &AExiaCharacterBase::EnableMovementCustom, 0.1f, false);
}

void AExiaCharacterBase::ResetJumpLock()
{
	bCanJump = true;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AExiaCharacterBase::UpdateBoostEnergy()
{
	// 0.1초당 소모량만큼 차감
	float ConsumeAmount = (CurrentStat.BoostConsumptionRate * 0.1f);
	CurrentGNParticles -= ConsumeAmount;
	
	// 부스트 지속 시간 확인
	float BoostTime = GetWorldTimerManager().GetTimerElapsed(BoostTimerHandle);
	
	// 현재 속도
	float CurrentSpeed = GetVelocity().Size2D();
	
	// 부스트 중단 조건 체크
	// 연료 소진 시 혹은 정지 시 부스트 종료
	if (CurrentGNParticles <= 0)
	{
		StopBoost();
		if (GEngine) GEngine->AddOnScreenDebugMessage(5, 2.0f, FColor::Red, TEXT("Boost OFF: No Fuel!"));
		return;
	}
	
	// 부스트 중단 조건2 멈췄을때
	if(CurrentSpeed < 50.0f)
	{
		StopBoost();
		if (GEngine) GEngine->AddOnScreenDebugMessage(6, 0.1f, FColor::Yellow, TEXT("Boost Warning: Speed Low"));
	}
	
	// 디버그 로그
	if (GEngine)
	{
		FString Msg = FString::Printf(TEXT("Using GN Particles: %.1f"), CurrentGNParticles);
		// 부스트 중일 때는 '빨간색'으로 표시
		GEngine->AddOnScreenDebugMessage(2, 0.1f, FColor::Red, Msg);
	}
}

void AExiaCharacterBase::RecoverGNParticles()
{
	float RecoveryRate = CurrentStat.BoostConsumptionRate * 0.8f;
	float RecoveryAmount = RecoveryRate * 0.1f;
	
	CurrentGNParticles += RecoveryAmount;
	
	// (디버그용)로그 출력
	if (GEngine)
	{
		FString Msg = FString::Printf(TEXT("Recovering... %.1f / %.1f"), CurrentGNParticles, CurrentStat.GNParticles);
		GEngine->AddOnScreenDebugMessage(3, 0.1f, FColor::Green, Msg);
	}
	
	// 만약 GN입자가 최대치일때 예외적 처리
	if (CurrentGNParticles >= CurrentStat.GNParticles)
	{
		CurrentGNParticles = CurrentStat.GNParticles;
		GetWorldTimerManager().ClearTimer(RecoveryTimerHandle);
        
		if (GEngine) GEngine->AddOnScreenDebugMessage(4, 2.0f, FColor::Cyan, TEXT("GN Particles Fully Charged!"));
	}
	
}

void AExiaCharacterBase::SetWeaponCollisionEnabled(bool bEnabled)
{
	if (WeaponCollision)
	{
		if (bEnabled)
		{
			WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			HitActors.Empty(); // 새로운 공격이니 목록 초기화
		}
		else
		{
			WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void AExiaCharacterBase::EnableMovementCustom()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AExiaCharacterBase::UpdateFlight(float DeltaTime)
{
	//비행 중 지속적으로 상승 힘 가하기
	FVector BoostForce = FVector::UpVector * JumpBoostForce;
	GetCharacterMovement()->AddForce(BoostForce);
	float CurrentHeight = GetActorLocation().Z;
	float MaxFlightHeight = 1000.f;
	
	if (CurrentHeight < MaxFlightHeight)
	{
		AddMovementInput(GetActorUpVector());
	}
	
	//TODO 연료 소모 로직 들어갈 자리 아직 미구현
}

void AExiaCharacterBase::CheckComboInput()
{
	// 저장된 입력이 있다면? -> 다음 콤보 실행!
	if (bHasSavedComboInput)
	{
		ProcessComboCommand();
	}
}

void AExiaCharacterBase::ProcessComboCommand()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	if (bIsStunned) return;

	if (AttackMontage && ComboNames.IsValidIndex(AttackComboCount))
	{
		FName TargetSection = ComboNames[AttackComboCount];
        
		// 몽타주 재생
		AnimInstance->Montage_Play(AttackMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(TargetSection, AttackMontage);

		// 애니메이션이 끝나면 OnAttackMontageEnded 함수가 실행되도록 연결
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AExiaCharacterBase::OnAttackMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
        
		// 콤보 카운트 증가
		AttackComboCount = (AttackComboCount + 1) % ComboNames.Num();
	}
	
	bIsAttacking = true;
	bHasSavedComboInput = false; // 입력 소모했으니 초기화
}

void AExiaCharacterBase::CheckNextCombo()
{
	// 저장된 입력이 없다면 그냥 종료
	if (!bHasBufferedInput) return;

	UE_LOG(LogTemp, Log, TEXT("--- CheckNextCombo 노티파이 발생 (예약됨: %s) ---"), 
		bHasBufferedInput ? TEXT("True") : TEXT("False"));
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !AttackMontage) return;
	
	// 다음 콤보 섹션이 있는지 확인하고 점프
	if (ComboNames.IsValidIndex(AttackComboCount))
	{
		FName TargetSection = ComboNames[AttackComboCount];
        
		// [로그] 실제로 콤보가 넘어가는지 확인용
		UE_LOG(LogTemp, Log, TEXT("CheckNextCombo: Next Section Found -> %s"), *TargetSection.ToString());

		AnimInstance->Montage_JumpToSection(TargetSection, AttackMontage);

		// 다음 공격을 위해 카운트 증가 및 예약 해제
		AttackComboCount++; 
		bHasBufferedInput = false;
        
		// 공격 중 상태 유지
		bIsAttacking = true;
	}
}


void AExiaCharacterBase::StopFlying()
{	
	bIsFlying = false;
	
	// 비행 종료 시 중력 원상 복구
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	
	// 낙하 시 기본 제동 수치로 복구
	GetCharacterMovement()->BrakingDecelerationFalling = 0.0f;
}

// 체력 퍼센트 반환
float AExiaCharacterBase::GetHPPercent_Implementation() const
{
	return (MaxHP > 0.0f) ? (CurrentHP / MaxHP) : 0.0f;
}

// 부스트(GN입자) 퍼센트 반환
float AExiaCharacterBase::GetGNParticlePercent_Implementation() const
{
	// CurrentStat.GNParticles가 최대치, CurrentGNParticles가 현재치입니다
	return (CurrentStat.GNParticles > 0.0f) ? (CurrentGNParticles / CurrentStat.GNParticles) : 0.0f;
}

// 가드 게이지 퍼센트 반환
float AExiaCharacterBase::GetGuardPercent_Implementation() const
{
	// 구조체 내의 CurrentGuardHP와 MaxGuardHP를 사용합니다
	return (CurrentStat.MaxGuardHP > 0.0f) ? (CurrentStat.CurrentGuardHP / CurrentStat.MaxGuardHP) : 0.0f;
}

// Called every frame
void AExiaCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 가드 중이 아니고, 경직 상태가 아니며, 가드 체력이 깍여 있다면 회복모드
	if (!bBlock && !bIsStunned && CurrentGuardHP < MaxGuardHP)
	{
		CurrentGuardHP = FMath::Min(CurrentGuardHP + (GuardRecoveryRate * DeltaTime), MaxGuardHP);
	}
	
	// // 이동 입력이 있고 + 부스트 키가 눌려있을 때만 bIsBoosting을 참으로 유지
	// bool bHasInput = GetLastMovementInputVector().Size() > 0.0f;
	// bIsBoosting = bIsBoostKeyDown && bHasInput; 
	
	if (bIsJumpBoosting)
	{
		FVector CurrentVel = GetCharacterMovement()->Velocity;
		
		if (CurrentVel.Z < -100.0f)
		{
			CurrentVel.Z = -100.0f;
			GetCharacterMovement()->Velocity = CurrentVel;
		}
	}
	
	// 연료 기능 구현하면서 중간 조건이 같이 추가되었으며 더이상 Tick에서 처리 하지 않음.
	// if (bIsBoosting)
	// {
	// 	// 1. 현재 속도 확인 (2D 평면 기준)
	// 	float CurrentSpeed = GetVelocity().Size2D();
	// 	
	// 	if (CurrentSpeed < 100.0f)
	// 	{
	// 		// 3. 입력이 아예 없거나 벽에 가로막힌 경우 대시 종료
	// 		StopBoost();
	// 		
	// 		// UE_LOG(LogTemp, Warning, TEXT("Dash Auto Stopped - Velocity too low"));
	// 	}
	// }
	
	if (GetCharacterMovement()->IsFalling() && bIsBoosting)
	{
		GetCharacterMovement()->AirControl = 2.0f;
		
		// 마찰력 증가 값
		GetCharacterMovement()->BrakingDecelerationFalling = 6000.0f;
	}
	else
	{
		GetCharacterMovement()->AirControl = 0.35f;
		GetCharacterMovement()->BrakingDecelerationFalling = 1000.0f;
	}
}

// Called to bind functionality to input
// void AExiaCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
// {
// 	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
// 	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
// 	{
// 		if (JumpAction)
// 		{
// 			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AExiaCharacterBase::StartJumpBoost);
// 			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AExiaCharacterBase::JumpBoosting);
// 			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AExiaCharacterBase::StopJumpBoost);
// 		}
// 		
// 		if (BoostAction)
// 		{
// 			EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Started, this, &AExiaCharacterBase::StartBoost);
// 			EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Triggered, this, &AExiaCharacterBase::Boosting);
// 			EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Completed, this, &AExiaCharacterBase::StopBoost);
// 		}
//
// 		// 이동 바인딩 ( 2026-01-19 이동이 안되던 문제는 해당 로직의 누락으로 확인되어 2026-01-20 오전에 해당 로직을 추가하여 해결되었음. )
// 		if (MoveAction)
// 		{
// 			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AExiaCharacterBase::Move);
// 		}
//
// 		// 회전 바인딩
// 		if (LookAction)
// 		{
// 			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AExiaCharacterBase::Look);
// 		}
// 	}
// }

void AExiaCharacterBase::SetCombatState(EGundamCombatState NewState)
{
	CombatState = NewState;
	
	if (CombatState == EGundamCombatState::Combat)
	{
		// 전투 상태 : 카메라를 대상에게 고정한 상태로 이동
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		// 비전투 상태 : 이동 방향으로 몸 회전
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void AExiaCharacterBase::StartJumpBoost()
{
	// 짧게 누를때 대응
	Jump(); 
	bIsJumpBoosting = true;
}

void AExiaCharacterBase::JumpBoosting()
{
	if (!bIsJumpBoosting) return;
	// 길게 누를때 대응
	if (bIsJumpBoosting && GetCharacterMovement()->IsFalling())
	{
		FVector JumpForce = FVector::UpVector * JumpBoostForce;
		GetCharacterMovement()->AddForce(JumpForce);
        
		// GN 입자 소모 로직 호출
		float DeltaTime = GetWorld()->GetDeltaSeconds();
		ConsumeGNParticles(DeltaTime); 
		
		GetCharacterMovement()->MaxAcceleration = 6000.0f;
	}
}

void AExiaCharacterBase::StopJumpBoost()
{
	//키를 떼면 점프 중단 및 부스트 상태 해제
	StopJumping();
	bIsJumpBoosting = false;
}

void AExiaCharacterBase::Jump()
{
	Super::Jump();
	
	if (!bCanJump || GetCharacterMovement()->IsFalling())
	{
		return;
	}
	
	bCanJump = false;
	bIsJumping = true;
	
	if (bCanJump && !GetCharacterMovement()->IsFalling() && !bHasJumped)
	{

		FVector JumpDir = GetLastMovementInputVector();
		float JumpUpForce = 6000.f;
		float JumpForwardForce = 400.f;
		
		FVector LaunchVel = (JumpDir * JumpForwardForce) + FVector(0,0, JumpUpForce);
		LaunchCharacter(LaunchVel, false, true);
	}
	else if (GetCharacterMovement()->IsFalling() && !bHasJumped)
	{
		StartJumpDash();
		bHasJumpDashUsed = true;
	}
}

void AExiaCharacterBase::StartJumpDash()
{
	if (!bIsJumpBoosting)
	{
		bIsJumpBoosting = true;
		
		GetCharacterMovement()->GravityScale = DefaultGravityScale - 2.5f;
		GetCharacterMovement()->MaxFlySpeed = CurrentStat.MoveSpeed * (BoostSpeedMultiplier * 1.5);
		
	}
}

void AExiaCharacterBase::StopJumpDash()
{
	bIsJumpBoosting = false;
	bIsJumping = false;
	
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
}

void AExiaCharacterBase::StartBoost()
{
	if (bIsBoosting) return;
	
	// 아무 키도 눌리지 않았다면 부스트를 실행하지 않음.
	FVector InputDir = GetLastMovementInputVector();
	if (InputDir.IsNearlyZero()) return;
	
	// 회복 타이머가 돌고 있었다면 중지시킨다. ( 동시 실행 방지 )
	GetWorldTimerManager().ClearTimer(RecoveryTimerHandle);
	
	// 부스팅 상태 전환
	bIsBoosting = true;
	
	// 소모 타이머 시작 0.1초마다 UpdateBoostEnegy 함수를 반복 실행
	GetWorldTimerManager().SetTimer(BoostTimerHandle, this, &AExiaCharacterBase::UpdateBoostEnergy, 0.1f, true);
	
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	
	FVector LaunchDir = GetLastMovementInputVector().GetSafeNormal();
	GetCharacterMovement()->RotationRate = FRotator(0,0,0);
	GetCharacterMovement()->MaxWalkSpeed = CurrentStat.MoveSpeed * BoostSpeedMultiplier;
	

	// [짧게 누르기 대응] 즉각적인 반응 필요
	if (GetCharacterMovement()->IsFalling())
	{
		LaunchCharacter(LaunchDir.GetSafeNormal() * 1000.0f, true, false);
	}
}

void AExiaCharacterBase::Boosting()
{
	if (bIsBoosting || GetCharacterMovement()->IsFalling()) return;
	
	// [길게 누르기 대응] 누르고 있는 동안 매 프레임 실행
	if (GetCharacterMovement()->IsFalling())
	{
		//공중 부스트 - 점프 부스트기준
		GetCharacterMovement()->AddInputVector(FVector::UpVector * 1.5f);
	}
	
	// GN입자 실시간 소모
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	
	ConsumeGNParticles(DeltaTime);
}

void AExiaCharacterBase::StopBoost()
{
	if (!bIsBoosting) return;
	
	bIsBoosting = false;
		
	// 소모 타이머 중지
	GetWorldTimerManager().ClearTimer(BoostTimerHandle);
	
	//속도 원복
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->RotationRate = FRotator(0,180.0f,0);
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->MaxWalkSpeed = CurrentStat.MoveSpeed;

	if (CurrentGNParticles < CurrentStat.GNParticles)
	{
		GetWorldTimerManager().SetTimer(RecoveryTimerHandle, this, &AExiaCharacterBase::RecoverGNParticles, 0.1f, true);
	}
}

void AExiaCharacterBase::ConsumeGNParticles(float DeltaTime)
{
	//d 추후 데이터 테이블의 CurrentGNParticles의 값을 깍는 로직이 들어갈 자리
	//if (CurrentStat.MaxGNParticles <= 0) StopBoost();
}

// void AExiaCharacterBase::Move(const FInputActionValue& Value)
// {
// 	FVector2D MovementVector = Value.Get<FVector2D>();
// 	
// 	if (bIsBraking) return;
//
// 	// 디버그용
// 	//UE_LOG(LogTemp, Log, TEXT("Move Input: X=%f, Y=%f"), MovementVector.X, MovementVector.Y);
// 	
// 	if (Controller != nullptr)
// 	{
// 		// 컨트롤러의 회전 방향
// 		const FRotator Rotation = Controller->GetControlRotation();
// 		const FRotator YawRotation(0, Rotation.Yaw, 0);
//
// 		// 방향 벡터
// 		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
// 		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
// 		
// 		AddMovementInput(ForwardDirection, MovementVector.Y);
// 		AddMovementInput(RightDirection, MovementVector.X);
// 		
// 		// 디버그용: 수치가 0이 아닌지 확인
// 		UE_LOG(LogTemp, Log, TEXT("MovementVector: %s"), *MovementVector.ToString());
// 	}
// }
//
// void AExiaCharacterBase::Look(const FInputActionValue& Value)
// {
// 	FVector2D LookAxisVector = Value.Get<FVector2d>();
// 	
// 	if (Controller != nullptr)
// 	{ 
// 		//마우스 좌우 움직임
// 		AddControllerYawInput(LookAxisVector.X);
// 		//마우스 상하 움직임
// 		AddControllerPitchInput(LookAxisVector.Y);
// 	}
// }

void AExiaCharacterBase::SetWarpTarget(AActor* TargetActor)
{
	if (MotionWarpingComp && TargetActor)
	{
		FVector MyLocation = GetActorLocation();
		FVector TargetLocation = TargetActor->GetActorLocation();

		// 적을 바라보는 정확한 회전값 계산
		// 내 위치에서 적의 위치를 바라보는 각도를 구합니다.
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetLocation);
		LookAtRot.Pitch = 0.0f; // 캐릭터가 위아래로 기울지 않게 고정
		LookAtRot.Roll = 0.0f;

		// 적의 몸 속으로 파고들지 않게 적 위치에서 내 쪽으로 살짝 오프셋(간격) 주기
		// 적과 나 사이의 방향 벡터를 구해서 약 60~80cm 정도 앞에서 멈추 도록 설계
		FVector Direction = (MyLocation - TargetLocation).GetSafeNormal();
		FVector WarpLocation = TargetLocation + (Direction * 250.0f); 

		// 워핑 타겟 업데이트
		MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(
			FName("AttackTarget"),
			WarpLocation,
			LookAtRot
		);
	}
	else if (MotionWarpingComp) // 타겟이 없으면?
	{
		// 워핑 타겟을 지워버립니다 (그냥 제자리 공격)
		MotionWarpingComp->RemoveWarpTarget(FName("AttackTarget"));
	}
}

void AExiaCharacterBase::StartBraking()
{
	// 감속력을 높이기
	GetCharacterMovement()->BrakingDecelerationWalking = 10000.0f;
	GetCharacterMovement()->GroundFriction = 10.0f;
	
	// 이동속도를 일시적으로 낮추기
	GetCharacterMovement()->MaxWalkSpeed = 2.0f;
}

void AExiaCharacterBase::StopBraking()
{
	// 원래 수치 복구
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;
	GetCharacterMovement()->GroundFriction = 8.0f;
	GetCharacterMovement()->MaxWalkSpeed = CurrentStat.MoveSpeed;
}
