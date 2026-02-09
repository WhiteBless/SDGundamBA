#include "BTService_CheckTargetAir.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_CheckTargetAir::UBTService_CheckTargetAir()
{
	NodeName = TEXT("Check If Target Is In Air");
	Interval = 0.2f; 
}

void UBTService_CheckTargetAir::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	// 블랙보드에서 플레이어 가져오기
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(GetSelectedBlackboardKey()));
	ACharacter* TargetChar = Cast<ACharacter>(TargetActor);

	if (TargetChar)
	{
		// 점프 중이거나, 비행 중이거나, 추락 중일 때 모두 true
		bool bIsInAir = TargetChar->GetCharacterMovement()->IsFalling();
		
		BB->SetValueAsBool(FName("bIsTargetInAir"), bIsInAir);
		
		UE_LOG(LogTemp, Log, TEXT("Target In Air: %s"), bIsInAir ? TEXT("TRUE") : TEXT("FALSE"));
	}
}