// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/IPHeroRangeAttack.h"

#include "Actor/IPProjectile.h"
#include "Interfaces/CombatInterface.h"

void UIPHeroRangeAttack::SpawnProjectile(bool bOverridePitch, float PitchOverride)
{
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo());
	
	FRotator Rotation = (GetAvatarActorFromActorInfo()->GetActorForwardVector()).Rotation();
	//UE_LOG(LogTemp, Warning, TEXT("SocketLocation: %s, ForwardVector: %s"),*SocketLocation.ToString(), *GetAvatarActorFromActorInfo()->GetActorForwardVector().ToString());
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());
		
	AIPProjectile* Projectile = GetWorld()->SpawnActorDeferred<AIPProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetAvatarActorFromActorInfo(),
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		
	Projectile->FinishSpawning(SpawnTransform);
}
