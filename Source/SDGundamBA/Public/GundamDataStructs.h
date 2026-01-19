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
	//생존 관련 스텟 정의
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHP;
	
	//이동 및 기동성 관련 스텟 정의
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Movement")
	float MoveSpeed;
	
	//부스터 정의
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Boost")
	float MaxGNParticles; //부스터 총량
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Boost")
	float BoostConsumptionReate;
	
	//스페셜 스킬 정의
	//트랜잠 활성 / 비활성 상태 정의
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Special")
	bool bHasTransAm;
	
	//트랜스 암 지속시작
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat | Special")
	float TransAmDuration;
	
	//생성자  -- 기본 값을 정의 --
	FGundamCharacterData()
		: MaxHP(2000.f), MoveSpeed(700.f), MaxGNParticles(100.f)
		, BoostConsumptionReate(20.f), bHasTransAm(false), TransAmDuration(15.f)
	{}
};
class SDGUNDAMBA_API GundamDataStructs
{
public:
	GundamDataStructs();
	~GundamDataStructs();
};
