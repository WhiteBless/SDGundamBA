// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GundamInterfaces.generated.h"


// 전투 상호작용 시 캡슐아 아닌 '피지컬 에셋' 부위별 타격을 고려한 설계 방식
UINTERFACE(MinimalAPI, Blueprintable)
class UGundamCombatInterface : public UInterface{GENERATED_BODY()};

class IGundamCombatInterface
{
	GENERATED_BODY()
	
public:
	//데미지 전달 (맞은 부위 뼈 이름 포함)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gundam | Combat")
	void ApplyGundamDamage(float DamageAmount, AActor* Attacker, FName HitBoneName);
	
	//공격 실행 명령
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gundam | Combat")
	void ExecuteAttack();
};

//상태 정보 인터페이스 HUD와 AI가 데이터를 안전하게 가져가게 할 목적
UINTERFACE(MinimalAPI, Blueprintable)
class UGundamStateInterface : public UInterface { GENERATED_BODY() };

class IGundamStateInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gundam | State")
	float GetHPPercent() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gundam | State")
	float GetGNParticlePercent() const;
};

UINTERFACE(MinimalAPI, Blueprintable)
class UPoolableInterface : public UInterface { GENERATED_BODY() };
//최적화를 위한 풀링 인터페이스 (혹시 몰라 정의만)
class IPoolableInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gundam | Optimization")
	void OnActivateFromPool();
};
