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

}

// Called when the game starts or when spawned
void AExiaCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	LoadCharacterData();
	
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

}

// Called to bind functionality to input
void AExiaCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// LookAction과 Look함수를 바인드
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AExiaCharacterBase::Look);
	}
}


void AExiaCharacterBase::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	// 디버그용
	UE_LOG(LogTemp, Log, TEXT("Move Input: X=%f, Y=%f"), MovementVector.X, MovementVector.Y);
	
	if (Controller != nullptr)
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
