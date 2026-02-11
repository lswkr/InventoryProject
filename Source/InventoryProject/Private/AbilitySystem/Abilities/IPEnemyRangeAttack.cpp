// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/IPEnemyRangeAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Actor/IPProjectile.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/EnemyInterface.h"

void UIPEnemyRangeAttack::SpawnProjectileForEnemy(const FVector& ProjectileTargetLocation,
                                                  const FVector& SocketLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	
	FRotator Rotation = GetAvatarActorFromActorInfo()->GetActorForwardVector().Rotation(); //(ProjectileTargetLocation - SocketLocation).Rotation();
	const FProjectileInfo ProjectileInfo = IEnemyInterface::Execute_GetProjectileInfo(GetAvatarActorFromActorInfo());
	
	if (ProjectileInfo.bPitchOverride)
	{
		Rotation.Pitch = ProjectileInfo.Pitch;
	}
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());
		
	AIPProjectile* Projectile = GetWorld()->SpawnActorDeferred<AIPProjectile>(
		ProjectileInfo.ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaultsForEnemy();
	Projectile->FinishSpawning(SpawnTransform);
}

FDamageEffectParams UIPEnemyRangeAttack::MakeDamageEffectParamsFromClassDefaultsForEnemy(AActor* TargetActor) const
{
	
	const FProjectileInfo ProjectileInfo = IEnemyInterface::Execute_GetProjectileInfo(GetAvatarActorFromActorInfo());
	
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = ProjectileInfo.DamageType;
	Params.DebuffChance = ProjectileInfo.DebuffChance;
	Params.DebuffDamage = ProjectileInfo.DebuffDamage;
	Params.DebuffDuration = ProjectileInfo.DebuffDuration;
	Params.DebuffFrequency = ProjectileInfo.DebuffFrequency;
	
	return Params;
}
