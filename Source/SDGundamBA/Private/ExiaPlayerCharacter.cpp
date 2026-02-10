// Fill out your copyright notice in the Description page of Project Settings.


#include "ExiaPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

AExiaPlayerCharacter::AExiaPlayerCharacter()
{
	// 1. 카메라 설정 (Base에서 가져옴)
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 500.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
}

void AExiaPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 2. 입력 매핑 컨텍스트 추가
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AExiaPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 부모의 설정이 있다면 호출 (지금은 비어있겠지만 관례상)
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 이동, 시선
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AExiaPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AExiaPlayerCharacter::Look);

		// 점프
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AExiaPlayerCharacter::Input_StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AExiaPlayerCharacter::Input_StopJump);

		// 공격 (Base 함수 호출) 
		//TODO 프로젝타일 원거리 공격 액션은 구현할때 호출하는 함수 변경 필수!
		//EnhancedInputComponent->BindAction(ProjectileAttackAction, ETriggerEvent::Started, this, &AExiaPlayerCharacter::Input_ExecuteAttack); 
		EnhancedInputComponent->BindAction(AttackComboAction, ETriggerEvent::Started, this, &AExiaPlayerCharacter::Input_ExecuteAttack);

		// 가드 (Base 함수 호출)
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Triggered, this, &AExiaPlayerCharacter::Input_StartGuard);
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Completed, this, &AExiaPlayerCharacter::Input_StopGuard);

		// 대시, 비행
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AExiaPlayerCharacter::Input_Dash);
		EnhancedInputComponent->BindAction(FlyAction, ETriggerEvent::Started, this, &AExiaPlayerCharacter::Input_Fly);
		
		
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AExiaPlayerCharacter::StartBoost);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &AExiaPlayerCharacter::StopBoost);
	
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AExiaPlayerCharacter::Input_Fire);
		}
	}
}

// --- 입력 처리 함수들 ---

void AExiaPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AExiaPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AExiaPlayerCharacter::Input_Fire(const FInputActionValue& Value)
{
	if (bIsStunned || bBlock || bIsAttacking || bIsJumping) return;\

	if (RangedAttackMontage)
	{
		if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(RangedAttackMontage))
		{
			PlayAnimMontage(RangedAttackMontage);
			
			// 발사체 생성 '노티파이'
		}
	}
	else
	{
		FireRangedWeapon(nullptr);
	}
}

// 아래 함수들은 Base의 기능을 호출하는 래퍼(Wrapper)입니다.
void AExiaPlayerCharacter::Input_StartJump()
{
	Jump();
	if (bIsFlying)
	{
		// 원하시면 여기에 상승 로직을 넣을 수 있습니다.
		// AddMovementInput(GetActorUpVector(), 1.0f); 
		return;
	}
	
	if (GetCharacterMovement()->IsFalling())
	{
		StartFlying();
        
		// (옵션) 비행 시작할 때 살짝 위로 띄워주면 자연스럽습니다.
		LaunchCharacter(FVector(0, 0, 500.0f), false, true);
        
		UE_LOG(LogTemp, Log, TEXT("Jump to Fly!"));
	}
	
	else
	{
		Jump();
	}
}

void AExiaPlayerCharacter::Input_StopJump() { StopJumping(); }
void AExiaPlayerCharacter::Input_ExecuteAttack() { ExecuteAttack_Implementation(); } // Base의 함수 호출 
void AExiaPlayerCharacter::Input_StartGuard() { StartGuard(); }       // Base의 함수 호출
void AExiaPlayerCharacter::Input_StopGuard() { StopGuard(); }         // Base의 함수 호출
void AExiaPlayerCharacter::Input_Dash()
{
	Boosting();
}          

void AExiaPlayerCharacter::Input_Fly() 
{ 
	// 비행 토글 로직 (Base의 변수 조작)
	if (bIsFlying) StopFlying();
	else StartFlying();
}

void AExiaPlayerCharacter::Input_StartBoost()
{
	StartBoost();
	if (GetCharacterMovement()->IsFalling() || bIsFlying)
	{
		// 아직 비행 모드가 아니라면 켭니다. (점프 -> 대시 -> 자동 비행)
		if (!bIsFlying) 
		{
			StartFlying();
		}

		// 순간 대시 발동 (회피)
		Boosting(); 
	}
	else
	{
		StartBoost(); 
	}
}

void AExiaPlayerCharacter::Input_StopBoost()
{
	StopBoost();
}

void AExiaPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

