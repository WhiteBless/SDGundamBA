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
};
