// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "IPProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class EProjectileDamagePolicy : uint8
{
	OnHit,
	OnBeginOverlap	
};

UCLASS()
class INVENTORYPROJECT_API AIPProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AIPProjectile();

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FDamageEffectParams DamageEffectParams;

	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void OnHit();
	virtual void Destroyed() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* Sphere;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY()
	UAudioComponent* LoopingSoundComponent;
	
private:
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;

		
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ImpactEffect;
	
	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere)
	USoundBase* LoopingSound;
	
	TArray<AActor*> OverlappedActors;

	UPROPERTY(EditAnywhere)
	EProjectileDamagePolicy ProjectileDamagePolicy = EProjectileDamagePolicy::OnHit;

	UPROPERTY(EditDefaultsOnly)
	float DamagePeriod = 1.f;
	
	float TimePassed =0.f;


	bool HandleProjectileDamage(AActor* SourceAvatarActor, AActor* OtherActor, FDamageEffectParams DamageEffectParams);
};
