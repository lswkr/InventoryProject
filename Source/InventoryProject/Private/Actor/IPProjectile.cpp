// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/IPProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/IPAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

AIPProjectile::AIPProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
//	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	Sphere->OnComponentHit.AddDynamic(this, &AIPProjectile::OnSphereHit);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AIPProjectile::OnSphereOverlap);
}

void AIPProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (ProjectileDamagePolicy ==  EProjectileDamagePolicy::OnBeginOverlap)
	{
		TimePassed += DeltaTime;
		
		if (TimePassed > DamagePeriod)
		{
			TimePassed = 0.f;
			AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

			for (AActor* OverlappedActor : OverlappedActors)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Overlapped actor: %s"),*OverlappedActor->GetActorNameOrLabel());
				HandleProjectileDamage(SourceAvatarActor,OverlappedActor, DamageEffectParams);
			}
			OverlappedActors.Empty();//데미지 줄 액터 비워서 주기마다 새로 데미지 주기	
		}
	}
}

void AIPProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
	if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
	{
		Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}

void AIPProjectile::OnHit()
{

}

void AIPProjectile::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	Super::Destroyed();
}

void AIPProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (DamageEffectParams.SourceAbilitySystemComponent == nullptr) return;

	if (OverlappedActors.Contains(OtherActor))
	{
		return;
	}
	OverlappedActors.AddUnique(OtherActor);
}

void AIPProjectile::OnSphereHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	

	if (!IsValid(OtherActor))
	{
		Destroy();
		return;
	}
	if (DamageEffectParams.SourceAbilitySystemComponent == nullptr)
	{
		Destroy();
		return;
	}
	AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

	if (!UIPAbilitySystemLibrary::IsHostile(OtherActor, SourceAvatarActor))
	{
		Destroy();
		return;
	}

	
	
	bool DamageApplied = HandleProjectileDamage(SourceAvatarActor, OtherActor, DamageEffectParams);
	
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
		DamageEffectParams.DeathImpulse = DeathImpulse;
	
		const bool bKnockback = FMath::RandRange(1,100) < DamageEffectParams.KnockbackChance;
		if (bKnockback)
		{
			FRotator Rotation = GetActorRotation();
			Rotation.Pitch = 45.f;
	
			const FVector KnockbackDirection = Rotation.Vector();
			const FVector KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
			DamageEffectParams.KnockbackForce = KnockbackForce;
		}
		if (LoopingSoundComponent)
		{
			LoopingSoundComponent->Stop();
			LoopingSoundComponent->DestroyComponent();
		}
	
		DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
		UIPAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
	}
	
	if (DamageApplied)
	{
		if (LoopingSoundComponent)
		{
			LoopingSoundComponent->Stop();
			LoopingSoundComponent->DestroyComponent();
		}
	}
	Destroy();
}

bool AIPProjectile::HandleProjectileDamage(AActor* SourceAvatarActor, AActor* OtherActor, FDamageEffectParams DEParams)
{
	if (!UIPAbilitySystemLibrary::IsHostile(OtherActor, SourceAvatarActor)) return false;
	
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		
		const FVector DeathImpulse = GetActorForwardVector() * DEParams.DeathImpulseMagnitude;
		DEParams.DeathImpulse = DeathImpulse;

		const bool bKnockback = FMath::RandRange(1, 100) < DEParams.KnockbackChance;
		if (bKnockback)
		{
			FRotator Rotation = GetActorRotation();
			Rotation.Pitch = 45.f;

			const FVector KnockbackDirection = Rotation.Vector();
			const FVector KnockbackForce = KnockbackDirection * DEParams.KnockbackForceMagnitude;
			DEParams.KnockbackForce = KnockbackForce;
		}

		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());

		DEParams.TargetAbilitySystemComponent = TargetASC;
		UIPAbilitySystemLibrary::ApplyDamageEffect(DEParams);
		return true;
	}
	return false;
}
