// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExiaCharacterBase.h"
#include "InputActionValue.h"
#include "ExiaPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
UCLASS()
class SDGUNDAMBA_API AExiaPlayerCharacter : public AExiaCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AExiaPlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* FireAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gundam | Combat")
	UAnimMontage* RangedAttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AttackComboAction; // 콤보 공격

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* GuardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FlyAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* StartBoostAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* StopBoostAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Input_Fire(const FInputActionValue& Value);
	
	// 점프, 공격, 가드 등은 부모(Base)의 함수를 그대로 호출하거나 여기서 감쌈
	void Input_StartJump();
	void Input_StopJump();
	void Input_ExecuteAttack();
	void Input_StartGuard();
	void Input_StopGuard();
	void Input_Dash();
	void Input_Fly();
	void Input_StartBoost();
	void Input_StopBoost();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
