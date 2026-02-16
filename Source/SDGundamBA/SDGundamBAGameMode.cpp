// Copyright Epic Games, Inc. All Rights Reserved.

#include "SDGundamBAGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void ASDGundamBAGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle SpawnTimer;
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASDGundamBAGameMode::DelayedSpawn, 3.0f, false);
}

void ASDGundamBAGameMode::DelayedSpawn()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 1. 플레이어 스폰
	if (PlayerClass)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		ACharacter* NewPlayer = World->SpawnActor<ACharacter>(PlayerClass, PlayerSpawnPos, FRotator::ZeroRotator, Params);
        
		// 빙의 (Possess)
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->Possess(NewPlayer);
		}
        
		// 이펙트 재생
		if (SpawnFX) UGameplayStatics::SpawnEmitterAtLocation(World, SpawnFX, PlayerSpawnPos);
	}

	// 2. AI 스폰
	if (AIClass)
	{
		World->SpawnActor<ACharacter>(AIClass, AISpawnPos, FRotator(0, 180, 0));
		if (SpawnFX) UGameplayStatics::SpawnEmitterAtLocation(World, SpawnFX, AISpawnPos);
	}
    
	// 3. UI에 "GAME START" 띄우기 (옵션)
}

ASDGundamBAGameMode::ASDGundamBAGameMode()
{

}

void ASDGundamBAGameMode::EndMission(bool bIsVictory)
{
	TSubclassOf<UUserWidget> WidgetClass = bIsVictory ? VictoryWidgetClass : DefeatWidgetClass;
	
	if (WidgetClass)
	{
		UUserWidget* EndWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
		if (EndWidget)
		{
			EndWidget->AddToViewport();
		}
	}
	
	// 마우스 커서 보이기 및 입력모드 전환 - UI 조작 목적
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->bShowMouseCursor = true;
		
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(nullptr);
		PC->SetInputMode(InputMode);
		
		UE_LOG(LogTemp, Warning, TEXT("Mission Ended! Result: %s"), bIsVictory ? TEXT("WIN") : TEXT("LOSE"));
	}
}

void ASDGundamBAGameMode::GameStop()
{
	//TODO 게임 스탑 조건문 들어가야함
	//현재 임시 동작 예외처리 안들어가 있음..!!
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}
