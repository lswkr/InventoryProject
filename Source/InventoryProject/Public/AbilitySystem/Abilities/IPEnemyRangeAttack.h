// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/IPDamageGameplayAbility.h"
#include "IPEnemyRangeAttack.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYPROJECT_API UIPEnemyRangeAttack : public UIPDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void SpawnProjectileForEnemy(const FVector& ProjectileTargetLocation, const FVector& SocketLocation);

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaultsForEnemy(AActor* TargetActor = nullptr) const;

};
