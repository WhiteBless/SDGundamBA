// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogSDGundamBA, Log, All);

namespace GundamCollision
{
	// 에디터에서 첫 번째로 만든 커스텀 채널이 보통 GameTraceChannel1입니다.
	constexpr ECollisionChannel BossEnemy = ECC_GameTraceChannel5;
	constexpr ECollisionChannel GundamPlayer = ECC_GameTraceChannel4;
}
