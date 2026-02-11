// ExiaProjectile.cpp
#include "AExiaProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GundamInterfaces.h" // 데미지 전달용

AExiaProjectile::AExiaProjectile()
{
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    
    CollisionComp->InitSphereRadius(15.0f);
    CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
    
    RootComponent = CollisionComp;

    // 충돌 델리게이트 연결
    if (CollisionComp)
    {
        CollisionComp->OnComponentHit.AddDynamic(this, &AExiaProjectile::OnHit);
    }

    // 무브먼트 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 3000.0f;
    ProjectileMovement->MaxSpeed = 5000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.0f;
    ProjectileMovement->bIsHomingProjectile = false; 
    ProjectileMovement->HomingAccelerationMagnitude = 5000.0f;

    // 파티클 설정
    ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
    ParticleComp->SetupAttachment(RootComponent); // 루트가 확정된 뒤 부착
    
    InitialLifeSpan = 3.0f;
}

void AExiaProjectile::BeginPlay()
{
    Super::BeginPlay();
    
    AActor* MyOwner = GetOwner();
    if (!MyOwner) MyOwner = GetInstigator();
    
    if (MyOwner && CollisionComp)
    {
        CollisionComp->IgnoreActorWhenMoving(MyOwner, true);

        if (UPrimitiveComponent* OwnerRoot = Cast<UPrimitiveComponent>(MyOwner->GetRootComponent()))
        {
            OwnerRoot->IgnoreActorWhenMoving(this, true);
        }
    }
}

void AExiaProjectile::SetHomingTarget(AActor* Target)
{
    if (Target && ProjectileMovement)
    {
        ProjectileMovement->HomingTargetComponent = Target->GetRootComponent();
        ProjectileMovement->bIsHomingProjectile = true;
    }
}

void AExiaProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 유효성 검사
    if (!OtherActor || !OtherComp) return;
    
    if (OtherActor == this || OtherActor == GetOwner() || OtherActor == GetInstigator()) 
    {
        return; 
    }
    
    // [디버깅] 충돌 로그 출력
    // FString OwnerName = GetOwner() ? GetOwner()->GetName() : TEXT("NULL");
    // UE_LOG(LogTemp, Warning, TEXT("[Projectile Debug] Hit Actor: %s | Hit Comp: %s | My Owner: %s"), 
    //     *OtherActor->GetName(), *OtherComp->GetName(), *OwnerName);
    
    AActor* SafeShooter = ShooterActor ? ShooterActor : GetOwner();
    
    // 최종 검증
    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherActor != SafeShooter))
    {
        // [로그] 어떤 뼈에 맞았는지 확인 가능
        // UE_LOG(LogTemp, Log, TEXT("Hit Bone: %s"), *Hit.BoneName.ToString());
        
        // 데미지 인터페이스 호출
        if (OtherActor->GetClass()->ImplementsInterface(UGundamCombatInterface::StaticClass()))
        {
            IGundamCombatInterface::Execute_ApplyGundamDamage(
                OtherActor, 
                DamageAmount, 
                SafeShooter, 
                Hit.BoneName,
                Hit.ImpactPoint
            );
        }

        // 타격 이펙트
        if (HitExplosionEffect)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitExplosionEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
        }

        //TODO 투사체 디스트로이 잠시 비활성화
        Destroy();
    }
}