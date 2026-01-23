// Fill out your copyright notice in the Description page of Project Settings.


#include "ExiaCharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionWarpingComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AExiaCharacterBase::AExiaCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 500.0f;
	SpringArmComp->bUsePawnControlRotation = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	
	MotionWarpingComp = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));

	JumpMaxHoldTime = 0.5f;
}

// Called when the game starts or when spawned
void AExiaCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	LoadCharacterData();
	
	GetCharacterMovement()->JumpZVelocity = 900.f;
	GetCharacterMovement()->GravityScale = 4.5f;
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext가 할당되지 않았습니다!"));
			}
		}
	}
}

void AExiaCharacterBase::LoadCharacterData()
{
	if (StatDataTable)
	{
		static const FString ContextString(TEXT("Exia Data Context"));
		FGundamCharacterData* DataRow = StatDataTable->FindRow<FGundamCharacterData>(FName("Exia_R2"), ContextString);
	
		if (DataRow)
		{
			CurrentStat = *DataRow;
			GetCharacterMovement()->MaxWalkSpeed = CurrentStat.MoveSpeed;
			UE_LOG(LogTemp, Warning, TEXT("Exia Data Loaded : HP = %f"), CurrentStat.MaxHP);
		}
	
	}
}

void AExiaCharacterBase::StartFlying()
{
	if (GetCharacterMovement())
	{
		// 비행 시작
		GetCharacterMovement()->GravityScale = FlightGravityScale + 2.0f;
		
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
	
	GetCharacterMovement()->DisableMovement(); // 잠시 멈춤
	GetWorldTimerManager().SetTimer(LandingTimerHandle, this, &AExiaCharacterBase::EnableMovementCustom, 0.1f, false);
}

void AExiaCharacterBase::ResetJumpLock()
{
	bCanJump = true;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
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
	
	//연료 소모 로직 들어갈 자리 아직 미구현
}


void AExiaCharacterBase::StopFlying()
{	
	// 비행 종료 시 중력 원상 복구
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	
	// 낙하 시 기본 제동 수치로 복구
	GetCharacterMovement()->BrakingDecelerationFalling = 0.0f;
}

float AExiaCharacterBase::GetHPPercent_Implementation() const
{
	return (CurrentStat.MaxHP > 0) ? (1.0f) : 0.0f; //현재 HP로직 추가 전 임시 반환
}

float AExiaCharacterBase::GetGNParticlePercent_Implementation() const
{
	return 1.0f; //임시 반환
}

// Called every frame
void AExiaCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
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
}

// Called to bind functionality to input
void AExiaCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AExiaCharacterBase::StartJumpBoost);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AExiaCharacterBase::JumpBoosting);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AExiaCharacterBase::StopJumpBoost);
		}
		
		if (BoostAction)
		{
			EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Started, this, &AExiaCharacterBase::StartBoost);
			EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Triggered, this, &AExiaCharacterBase::Boosting);
			EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Completed, this, &AExiaCharacterBase::StopBoost);
		}

		// 이동 바인딩 ( 2026-01-19 이동이 안되던 문제는 해당 로직의 누락으로 확인되어 2026-01-20 오전에 해당 로직을 추가하여 해결되었음. )
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AExiaCharacterBase::Move);
		}

		// 회전 바인딩
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AExiaCharacterBase::Look);
		}
	}
}

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
		
		GetCharacterMovement()->MaxAcceleration = 5000.0f;
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
		float JumpUpForce = 800.f;
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
		
		GetCharacterMovement()->GravityScale = 0.00000000001f;
		GetCharacterMovement()->AirControl = 0.8f;
		GetCharacterMovement()->MaxFlySpeed = CurrentStat.MoveSpeed * (BoostSpeedMultiplier * 1.5);
		
	}
}

void AExiaCharacterBase::StopJumpDash()
{
	bIsJumpBoosting = false;
	bIsJumping = false;
	
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.05f;
}

void AExiaCharacterBase::StartBoost()
{
	// 이동 입력 가져오기
	FVector InputDir = GetLastMovementInputVector();
	
	// 아무 키도 눌리지 않았다면 부스트를 실행하지 않음.
	if (InputDir.IsNearlyZero()) return;
	
	bIsBoosting = true;
	
	bUseControllerRotationYaw = true;
	
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
	if (!bIsBoosting) return;
	
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
	bIsBoosting = false;

	bUseControllerRotationYaw = false;
	//속도 원복
	GetCharacterMovement()->RotationRate = FRotator(0,180.0f,0);
	
	GetCharacterMovement()->MaxWalkSpeed = CurrentStat.MoveSpeed;
}

void AExiaCharacterBase::ConsumeGNParticles(float DeltaTime)
{
	//d 추후 데이터 테이블의 CurrentGNParticles의 값을 깍는 로직이 들어갈 자리
	if (CurrentStat.MaxGNParticles <= 0) StopBoost();
}

void AExiaCharacterBase::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (bIsBraking) return;

	// 디버그용
	UE_LOG(LogTemp, Log, TEXT("Move Input: X=%f, Y=%f"), MovementVector.X, MovementVector.Y);
	
	if (Controller != nullptr != 0.0f)
	{
		// 컨트롤러의 회전 방향
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 방향 벡터
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
		
		// 디버그용: 수치가 0이 아닌지 확인
		UE_LOG(LogTemp, Log, TEXT("MovementVector: %s"), *MovementVector.ToString());
	}
}

void AExiaCharacterBase::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2d>();
	
	if (Controller != nullptr)
	{ 
		//마우스 좌우 움직임
		AddControllerYawInput(LookAxisVector.X);
		//마우스 상하 움직임
		AddControllerPitchInput(LookAxisVector.Y);
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
