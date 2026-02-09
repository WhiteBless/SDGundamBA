#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_CheckTargetAir.generated.h"

UCLASS()
class SDGUNDAMBA_API UBTService_CheckTargetAir : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_CheckTargetAir();

protected:
	// 주기적으로 실행될 함수 선언 (Tick과 같은 역할)
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};