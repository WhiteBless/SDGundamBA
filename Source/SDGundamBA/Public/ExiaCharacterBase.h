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
	
protected:
	// 콤보 섹션 이름 배열 (블루프린트의 ComboNames 대응) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gundam | Combat")
	TArray<FName> ComboNames;

	// 현재 콤보 카운트 (블루프린트의 AttackComboCount 대응) [cite: 5]
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gundam | Combat")
	int32 AttackComboCount = 0;

	// 공격 중 여부 (블루프린트의 Attacking 대응) [cite: 15]
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gundam | Combat")
	bool bIsAttacking = false;

	// 공격 몽타주 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gundam | Combat")
	class UAnimMontage* AttackMontage;

public:
	// 블루프린트에서 호출할 공격 실행 함수
	UFUNCTION(BlueprintCallable, Category = "Gundam | Combat")
	//void ExecuteAttack();
	virtual void ExecuteAttack_Implementation() override;

	// 콤보 리셋 함수 (블루프린트의 ResettingComboAttack 대응)
	UFUNCTION(BlueprintCallable, Category = "Gundam | Combat")
	void ResettingComboAttack();
	
protected:
	bool bHasBufferedInput;
	bool bIsBufferWindowOpen;

public:
	// Sets default values for this character's properties
	AExiaCharacterBase();

	void OpenInputBuffer();
	void CloseInputBuffer();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// 무기 판정용 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UBoxComponent* WeaponCollision;
	
	// 히트된 적들을 기억하는 배열 (중복 타격 방지용)
	UPROPERTY()
	TArray<AActor*> HitActors;
	
	// 컴포넌트 부착
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArmComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* CameraComp;
	
	// 모션 와핑
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Motion Warping")
	class UMotionWarpingComponent* MotionWarpingComp;
	
	// 데이터 관련
	UPROPERTY(VisibleDefaultsOnly, Category = "Data")
	class UDataTable* StatDataTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	FGundamCharacterData CurrentStat;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;
	
	//부스트 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* BoostAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* JumpAction;
	
	//부스트 설정 수치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float BoostSpeedMultiplier = 1.5f; // 일반 속도의 2배로 설정
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float VerticalBoostForce = 500.0f; // 점프 파워
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float JumpBoostForce = 200000.0f; // 상승 추력
	
	//GN입자
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentGNParticles;
	
	FTimerHandle BoostTimerHandle;
	void UpdateBoostEnergy(); // 연료 차감 전용 함수

	FTimerHandle RecoveryTimerHandle;
	void RecoverGNParticles(); // 연료 회복 전용 함수
public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE bool IsBoosting() const { return bIsBoosting; }
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE bool IsJumpBoosting() const { return bIsJumpBoosting; }
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE bool IsJumping() const { return bIsJumping; }
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
	
	void Jump();
	void StartJumpDash();
	void StopJumpDash();

	//부스트 관련 함수 정의
	void StartBoost();	//누르기 시작
	void Boosting();	// 누르는 도중
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopBoost();	// 떼는 그 시점
	
	//GN 입자(스테미나) 소모 로직 (추후 상세 구현 예정)
	void ConsumeGNParticles(float DeltaTime);
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
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
	// 데이터 테이블 에셋을 에디터에서 지정할 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* CharacterDataTable;

	// 데이터 테이블 행 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	FName CharacterRowName;

	// --- 실제 사용할 스탯 변수들 ---
    
	// 변하는 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentHP;

	// 최대 체력 (데이터 테이블에서 가져옴)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MaxHP;
	
	// 피격 몽타주 변수 
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* HitMontage;
	
public:
	virtual void ApplyGundamDamage_Implementation(float DamageAmount, AActor* Attacker, FName HitBoneName, FVector HitLocation) override;
	
protected:
	// 가드 상태 정의
	UPROPERTY(BlueprintReadWrite, Category = "States")
	bool bBlock;
	
	void BlockingStateStart();
	void BlockingStateEnd();
	
	// 비행 중 적용할 중력 값
	UPROPERTY(EditAnywhere, Category = "Movement")
	float FlightGravityScale = 0.4f;
	
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
	// 인터페이스 구현 ( 상태 정보 확인 )
	virtual float GetHPPercent_Implementation() const override;
	virtual float GetGNParticlePercent_Implementation() const override;
	
	// 데이터 로드 함수
	void LoadCharacterData();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EGundamCombatState CombatState = EGundamCombatState::Exploring;

	void SetCombatState(EGundamCombatState NewState);
};
