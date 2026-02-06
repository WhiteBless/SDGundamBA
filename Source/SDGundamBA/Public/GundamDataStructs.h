// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GundamDataStructs.generated.h"

/**
 * 엑시아 및 모든 기체의 공통 스탯 구조체를 설계하는 공간
 */
USTRUCT(BlueprintType)
struct FGundamCharacterData : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	//최대 가드 내구도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Guard")
	float MaxGuardHP;
	
	//현재 가드 내구도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Guard")
	float CurrentGuardHP;
	
	//가드 회복 속도 (초당)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Guard")
	float GuardRecoveryRate;
	
	//생존 관련 스텟 정의
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHP;
	
	//이동 및 기동성 관련 스텟 정의
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Movement")
	float MoveSpeed;
	
	//부스터 정의
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Boost")
	float GNParticles; //부스터 총량
	
// 기본 공격력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Combat")
    float AttackPower;

    // 기본 방어력 (데미지 차감 수치)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Combat")
    float DefensePower = 10.0f;

    // 가드 시 데미지 감소율 (0.0 ~ 1.0) 
    // 예: 0.3이면 30%만 받고 70%는 막음. 0.0이면 완전 방어.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Combat")
    float GuardDamageReduction;
	
	//스페셜 스킬 정의
	//트랜잠 활성 / 비활성 상태 정의
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Special")
	bool bHasTransAm;
	
	//트랜스 암 지속시작
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Special")
	float TransAmDuration;
	
	//이동 관련 상세 스텟
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Movement")
	float JumpZVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Movement")
	float DefaultGravityScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Boost")
	float BoostSpeedMultiplier;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Boost")
	float BoostConsumptionRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Boost")
	float FlightGravityScale = 0.5f;
	
	//생성자  -- 기본 값을 정의 --
	FGundamCharacterData()
		: MaxHP(2000.f), MoveSpeed(700.f), GNParticles(300.f), JumpZVelocity(900.f), BoostSpeedMultiplier(2.5f)
		, BoostConsumptionRate(20.f), bHasTransAm(false), TransAmDuration(15.f), DefaultGravityScale(4.5)
		, AttackPower(10), DefensePower(2.2), GuardDamageReduction(0.5)
		, MaxGuardHP(1000.f), CurrentGuardHP(1000.f), GuardRecoveryRate(20.f)
	{}
};
class SDGUNDAMBA_API GundamDataStructs
{
public:
	GundamDataStructs();
	~GundamDataStructs();
};
