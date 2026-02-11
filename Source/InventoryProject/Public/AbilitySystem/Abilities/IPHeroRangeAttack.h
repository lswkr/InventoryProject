// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/IPDamageGameplayAbility.h"
#include "IPHeroRangeAttack.generated.h"

class AIPProjectile;
/**
 * 
 */
UCLASS()
class INVENTORYPROJECT_API UIPHeroRangeAttack : public UIPDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(bool bOverridePitch = false, float PitchOverride = 0.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AIPProjectile> ProjectileClass;
};
