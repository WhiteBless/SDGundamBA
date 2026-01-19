// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GundamInterfaces.h" // 인터페이스 포함
#include "GundamDataStructs.h" // 구조체 포함
#include "InputActionValue.h"
#include "ExiaCharacterBase.generated.h"

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
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	// --- 애니메이션 연동 변수 ---
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float ForwardVelocity;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float RightVelocity;
	
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

};
