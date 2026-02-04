#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ExiaAnimInstance.generated.h"

class AExiaCharacterBase;

UCLASS()
class SDGUNDAMBA_API UExiaAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation (float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LocalVelocityForward;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LocalVelocityRight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsBoosting;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float LastDashForward;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LastDashRight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsFalling; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsJumping; 
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bCanJump;
	
public:

	// C++에서 미리 주인을 찾아놓고 여기에 저장합니다.
	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	AExiaCharacterBase* OwnerCharacter;

	// 블로킹(가드) 상태도 C++에서 처리합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State")
	bool bAnim_Blocking;
	
public:
	bool bIsAscending;
};