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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LocalVelocityForward;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LocalVelocityRight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsBoosting;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LastDashForward;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LastDashRight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsFalling; // 공중에 떠 있는지 여부 (점프/추락 포함)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsJumping; // 점프 버튼이 눌렸는지 여부
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bCanJump;
	
public:
	bool bIsAscending;
};
