// Copyright Epic Games, Inc. All Rights Reserved.

#include "SDGundamBAGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

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
	//현재 안들어가 있음..!!
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}
