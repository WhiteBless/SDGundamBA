
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AExiaProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;

UCLASS()
class SDGUNDAMBA_API AExiaProjectile : public AActor
{
	GENERATED_BODY()
    
public:    
	AExiaProjectile();

protected:
	virtual void BeginPlay() override;

public:
	
	// 충돌 판정 구체
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	USphereComponent* CollisionComp;

	// 투사체 움직임 제어 (유도 기능 포함)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

	// 투사체 이펙트 (빔 효과)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FX")
	UParticleSystemComponent* ParticleComp;

	// 타격 시 터지는 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	UParticleSystem* HitExplosionEffect;

	// 데미지 수치
	float DamageAmount = 50.0f;

	// 발사한 주인 (자폭 방지용)
	AActor* ShooterActor;

	// [핵심] 유도 대상 설정 함수
	void SetHomingTarget(AActor* Target);

	// 충돌 시 호출될 함수
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};