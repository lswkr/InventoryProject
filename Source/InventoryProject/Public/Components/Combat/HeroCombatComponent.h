// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "HeroCombatComponent.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYPROJECT_API UHeroCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

public:
	virtual void OnHitTargetActor(AActor* HitActor) override;
	virtual void OnHitBoxPulledFromTargetActor(AActor* HitActor) override;

	// virtual void ToggleBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType) override;
	// virtual void ToggleSpecialAttackBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType) override;
	//
};
