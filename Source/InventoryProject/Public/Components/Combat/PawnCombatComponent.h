// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "PawnCombatComponent.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EToggleDamageType : uint8
{
	Left,
	Right,
	Body
};

UCLASS()
class INVENTORYPROJECT_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	virtual void OnHitTargetActor(AActor* HitActor);
	virtual void OnHitBoxPulledFromTargetActor(AActor* HitActor);

protected:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void ToggleBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void ToggleSpecialAttackBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType);

	UPROPERTY()
	TArray<AActor*> OverlappedActors;

};
