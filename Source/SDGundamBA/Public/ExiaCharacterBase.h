#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GundamInterfaces.h" // 인터페이스 포함
#include "GundamDataStructs.h" // 구조체 포함
#include "InputActionValue.h"
#include "ExiaAnimInstance.h"
#include "MotionWarpingComponent.h"
#include "ExiaCharacterBase.generated.h"

UENUM(Blueprintable)
enum class EGundamCombatState : uint8
{
	Exploring	UMETA(DisplayName = "Exploring"),
	Combat		UMETA(DisplayName = "Combat")
};


UCLASS()
class SDGUNDAMBA_API AExiaCharacterBase : public ACharacter, public IGundamCombatInterface, public IGundamStateInterface
{
	GENERATED_BODY()
public:
	// [Sound] 파일명(String)으로 사운드 재생
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlaySoundByName(FString SoundName);

	// [Combat] 원거리 공격 중인가? (AI 감지용)
	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsRangedAttacking() const { return bIsRangedAttacking; }

	// [Combat] 원거리 공격 상태 변경 (애니메이션 노티파이에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetRangedAttacking(bool bStatus) { bIsRangedAttacking = bStatus; }

protected:
	// [Audio] 사운드 저장소 (에디터에서 설정: "Attack" -> SoundCue)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TMap<FString, USoundBase*> SoundLibrary;

	// [Combat] 원거리 공격 상태 플래그
	bool bIsRangedAttacking = false;

	//TODO [Combat] 넉백 힘 (기본값 1200)
	UPROPERTY(EditAnywhere, Category = "Combat")
	float KnockbackStrength = 1200.0f;

public:
	// 데미지 처리 함수 오버라이드 (넉백 구현)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
protected:
	// 콤보 섹션 이름 배열 (블루프린트의 ComboNames 대응) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gundam | Combat")
	TArray<FName> ComboNames;

	// 현재 콤보 카운트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gundam | Combat")
	int32 AttackComboCount = 0;

public:
	// 공격 중 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gundam | Combat")
	bool bIsAttacking = false;
	
protected:
	//가드 콜리전 박스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UBoxComponent* GuardCollision;
	
	// 공격 몽타주 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gundam | Combat")
	class UAnimMontage* AttackMontage;
	
	// 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Combat")
	float AttackPower = 100.0f;

	// 방어력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Combat")
	float DefensePower = 10.0f;

	// 가드 데미지 감소율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Combat")
	float GuardDamageReduction = 0.5f;

public:
	// 블루프린트에서 호출할 공격 실행 함수
	virtual void ExecuteAttack_Implementation() override; //void ExecuteAttack();

	// 콤보 리셋 함수 (블루프린트의 ResettingComboAttack 대응)
	UFUNCTION(BlueprintCallable, Category = "Gundam | Combat")
	void ResettingComboAttack();
	
	// 데미지 처리
	virtual void ApplyGundamDamage_Implementation(float DamageAmount, AActor* Attacker, FName HitBoneName, FVector HitLocation) override;
	
	//TODO AI캐릭터 행동트리
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* AIBehaviorTree;
	
protected:
	bool bHasBufferedInput;
	bool bIsBufferWindowOpen;
	bool bForceBufferInput = false;

public:
	// Sets default values for this character's properties
	AExiaCharacterBase();

	void OpenInputBuffer();
	void CloseInputBuffer();
	
	//TODO 카메라 쉐이크
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | FX")
	TSubclassOf<class UCameraShakeBase> HitCameraShakeClass;

	//TODO 변수의 값을 외부 서버스 또는 태스크에서 읽어올 수 있도록 함수선언
	FORCEINLINE bool GetIsAttacking() const { return bIsAttacking; }
	FORCEINLINE bool GetIsBlocking() const { return bBlock; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gundam | Combat")
	bool bCanPlayHitReaction = false;
	
	// 가드 이펙트용 스태틱 메쉬 (GN 필드)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* GuardShieldMesh;

	// 가드 쿨타임 (가드를 풀고 나서 다시 올리기까지 걸리는 시간)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gundam | Combat")
	float GuardCooldownTime = 0.5f;

	// 현재 가드 가능 여부 (쿨타임 체크용)
	bool bCanGuard = true;

	// 쿨타임 타이머 핸들
	FTimerHandle GuardCooldownTimer;

	// 쿨타임 종료 후 실행될 함수
	void ResetGuardCooldown();
	
	// AI와 플레이어 공용 가드 함수
	UFUNCTION(BlueprintCallable, Category = "Gundam | Combat")
	void StartGuard();

	UFUNCTION(BlueprintCallable, Category = "Gundam | Combat")
	void StopGuard();
	
protected:
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	// 발사할 투사체 클래스 (블루프린트에서 BP_ExiaProjectile 할당)
	UPROPERTY(EditAnywhere, Category = "Gundam | Combat")
	TSubclassOf<class AExiaProjectile> RangedProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Gundam | Combat")
	FName MuzzleSocketName = FName("FirePosition");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gundam | FX")
	UAnimMontage* DeathMontage;
	
	UPROPERTY(EditAnywhere, Category = "Gundam | FX")
	FName DeathLoopSectionName = FName("Death");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gundam | FX")
	UParticleSystem* DeathExplosionEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gundam | FX")
	USoundBase* DeathExplosionSound;
	
	FTimerHandle DeathTimerHandle;
	
	UFUNCTION(BlueprintCallable, Category = "Gundam | Dead")
	void PlayDeathExplosion();
	
	UFUNCTION(BlueprintCallable, Category = "Gundam | Dead")
	void OnDeath();
	
	UFUNCTION(BlueprintCallable, Category = "Gundam | Dead")
	void DeathMontagePlay();
	
public:
	UFUNCTION(BlueprintCallable, Category = "Gundam | Combat")
	void FireRangedWeapon(AActor* Target = nullptr);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> DamageTextWidgetClass;

	// 데미지 텍스트 띄우는 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowDamageText(float Damage, FVector Location);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Guard")
	float MaxGuardHP;

	// 현재 가드 내구도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat | Guard")
	float CurrentGuardHP;

	// 가드 회복 속도 (초당 회복량)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Guard")
	float GuardRecoveryRate = 4.0f;

	// 가드 브레이크 시 경직 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Guard")
	float StunDuration = 2.0f;
	
	// 현재 경직(가드 브레이크) 상태인가?
	bool bIsStunned = false;
	
	// 가드 브레이크 모션 (비틀거리는 모션)
	UPROPERTY(EditAnywhere, Category = "Animation | Combat")
	UAnimMontage* GuardBreakMontage;
	
	// 가드 브레이크 발생 처리
	void OnGuardBreak();

	// 경직 회복 처리
	void RecoverFromStun();

	// 가드 게이지 회복 타이머용
	void RegenerateGuardHP(float DeltaTime);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// 무기 판정용 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UBoxComponent* WeaponCollision;
	
	// 히트된 적들을 기억하는 배열 (중복 타격 방지용)
	UPROPERTY()
	TArray<AActor*> HitActors;

	// 공격 적중 시 재생할 파티클 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gundam | FX")
	UParticleSystem* HitImpactEffect;

	// 타격 사운드 - 나중에 적용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gundam | FX")
	USoundBase* HitImpactSound;
	
	// 모션 와핑
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Motion Warping")
	class UMotionWarpingComponent* MotionWarpingComp;
	
	// 데이터 관련
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	class UDataTable* StatDataTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	FGundamCharacterData CurrentStat;
	
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	// class UInputMappingContext* DefaultMappingContext;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	// class UInputAction* MoveAction;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	// class UInputAction* LookAction;
	//
	// //부스트 입력 액션
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	// class UInputAction* BoostAction;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	// class UInputAction* JumpAction;
	
	//부스트 설정 수치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float BoostSpeedMultiplier = 1.5f; // 일반 속도의 2배로 설정
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float VerticalBoostForce = 500.0f; // 점프 파워
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float JumpBoostForce = 20000.0f; // 상승 추력
	
	//GN입자
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentGNParticles;
	
	void ConsumeGNParticles(float Amount); 
	void RecoverGNParticles(float DeltaTime);
	
	// FTimerHandle BoostTimerHandle;
	// void UpdateBoostEnergy(); // 연료 차감 전용 함수
	//
	// FTimerHandle RecoveryTimerHandle;
	// void RecoverGNParticles(); // 연료 회복 전용 함수
public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE bool IsBoosting() const { return bIsBoosting; }
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE bool IsJumpBoosting() const { return bIsJumpBoosting; }
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE bool IsJumping() const { return bIsJumping; }
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE bool IsDeath() const { return bIsDeath; }
	//상태 변수
	bool bIsJumping;
	
	// 점프 가능 여부 (착지 후 딜레이용)
	bool bCanJump = true;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetWeaponCollisionEnabled(bool bEnabled);
	
	UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
						 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						 bool bFromSweep, const FHitResult& SweepResult);
	
protected:
	// 가드 몽타주
	UPROPERTY(EditAnywhere, Category = "Animation | Combat")
	UAnimMontage* GuardMontage;

	// 가드 루프 섹션 이름 (예: "GuardLoop")
	UPROPERTY(EditAnywhere, Category = "Animation | Combat")
	FName GuardLoopSectionName = FName("GuardLoop");
	
	bool bIsBoosting = false;
	bool bIsJumpBoosting = false;
	
	FTimerHandle LandingTimerHandle;
	
	void EnableMovementCustom();
	
	// 사용자가 부스트 키(Shift)를 누르고 있는지 여부
	bool bIsBoostKeyDown = false;

	// 점프 가능 상태로 복구하는 함수
	void ResetJumpLock();
	
	//점프 부스트 관련 함수 정의
	void StartJumpBoost(); // Started
	void JumpBoosting();  // Triggered (상승 유지)
	void StopJumpBoost();  // Completed
	
	// 비행모드 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gundam | Movement")
	bool bIsFlying = false;
	
	void Jump();
	void StartJumpDash();
	void StopJumpDash();

	//부스트 관련 함수 정의
	void StartBoost();	//누르기 시작
	void Boosting();	// 누르는 도중
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopBoost();	// 떼는 그 시점
	
	// void Move(const FInputActionValue& Value);
	// void Look(const FInputActionValue& Value);
	
	// --- 애니메이션 연동 변수 ---
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float ForwardVelocity;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float RightVelocity;
	
	
public:
	//타겟 지정 함수 락온등에서 이용 예정
	void SetWarpTarget(AActor* TargetActor);
	
	// 제동 관련 선언
	void StartBraking();
	void StopBraking();

	bool bIsBraking;
	
protected:
	// 데이터 테이블 행 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	FName CharacterRowName;
    
	// 변하는 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentHP;

	// 최대 체력 (데이터 테이블에서 가져옴)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MaxHP;
	
	// 피격 몽타주 변수 
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* HitMontage;

protected:
	// 가드 상태 정의
	UPROPERTY(BlueprintReadWrite, Category = "States")
	bool bBlock;
	
	// 사망 판정
	UPROPERTY(BlueprintReadWrite, Category = "States")
	bool bIsDeath = false;
	
	
	// 가드 콜리전에 무언가 부딪혔을 때 실행될 함수
	UFUNCTION()
	void OnGuardOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);
	
	void BlockingStateStart();
	void BlockingStateEnd();
	
	// 비행 중 적용할 중력 값
	UPROPERTY(EditAnywhere, Category = "Movement")
	float FlightGravityScale = 1.0f;
	
	//기본 중력 값 정의
	float DefaultGravityScale = 2.5f;
	
	void StartFlying();
	void StopFlying();
	
	//점프 중 중복 점프가 안돼도록 판단하기 위해 선언
	bool bHasJumped = false;
	bool bHasJumpDashUsed = false;
	
	virtual void Landed(const FHitResult& Hit) override;
	
	// 매 프레임 연료 체크 및 추력 적용
	void UpdateFlight(float DeltaTime);
public:
	// 노티파이에서 호출할 함수
	void CheckComboInput();
	void ProcessComboCommand();
	
	UFUNCTION(BlueprintCallable, Category = "Gundam | Combat")
	void CheckNextCombo();

	UFUNCTION(BlueprintCallable, Category = "Stat")
	float GetMoveSpeed() const { return CurrentStat.MoveSpeed; }
	
protected:
	// 공격 입력이 들어왔었는지 저장하는 플래그
	bool bHasSavedComboInput;
	
	// 부스터 이펙트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* ThrusterFXComponent;

	// [추가] 매 프레임 이펙트 상태를 갱신하는 함수
	void UpdateThrusterEffect();
	
public:
	// 인터페이스 구현
	virtual float GetHPPercent_Implementation() const override; 
	virtual float GetGNParticlePercent_Implementation() const override;
	virtual float GetGuardPercent_Implementation() const override;
	
	// 데이터 로드 함수
	void LoadCharacterData();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EGundamCombatState CombatState = EGundamCombatState::Exploring;

	void SetCombatState(EGundamCombatState NewState);
};
