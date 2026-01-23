#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GundamInterfaces.h" // 인터페이스 포함
#include "GundamDataStructs.h" // 구조체 포함
#include "InputActionValue.h"
#include "ExiaAnimInstance.h"
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
	// Sets default values for this character's properties
	AExiaCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// 컴포넌트 부착
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArmComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* CameraComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
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
	float JumpBoostForce = 110000.0f; // 상승 추력
	
public:
	FORCEINLINE bool IsBoosting() const { return bIsBoosting; }
	//상태 변수
	
	bool bIsJumping;
	
	// 점프 가능 여부 (착지 후 딜레이용)
	bool bCanJump = true;
	
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
	void StopBoost();	// 떼는 그 시점
	
	//GN 입자(스테미나) 소모 로직 (추후 상세 구현 예정)
	void ConsumeGNParticles(float DeltaTime);
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	// --- 애니메이션 연동 변수 ---
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float ForwardVelocity;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float RightVelocity;\
	
public:
	// 제동 관련 선언
	void StartBraking();
	void StopBraking();

	bool bIsBraking;
	
protected:
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
