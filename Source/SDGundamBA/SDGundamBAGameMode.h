// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SDGundamBAGameMode.generated.h"
class ACharacter;
class UUserWidget;
/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class ASDGundamBAGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASDGundamBAGameMode();

	virtual void BeginPlay() override;
	
	void DelayedSpawn();
	
	// 에디터에서 지정할 스폰 위치 (Location)
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	FVector PlayerSpawnPos;
    
	UPROPERTY(EditAnywhere, Category = "Spawn")
	FVector AISpawnPos;
	// 스폰 이펙트 (파티클)
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	UParticleSystem* SpawnFX;

	// 스폰할 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TSubclassOf<ACharacter> PlayerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TSubclassOf<ACharacter> AIClass;
	
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	TSubclassOf<class UUserWidget> VictoryWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	TSubclassOf<class UUserWidget> DefeatWidgetClass;
	
	UFUNCTION(BlueprintCallable, Category= "GameRule")
	void EndMission(bool bIsVictory);
	
	UFUNCTION(BlueprintCallable, Category= "GameRule")
	void GameStop();
};



