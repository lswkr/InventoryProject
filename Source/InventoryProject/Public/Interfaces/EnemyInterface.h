// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};


USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AIPProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly)
	float DebuffChance = 0.f;
	
	UPROPERTY(EditDefaultsOnly)
	float DebuffDamage = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float DebuffDuration = 0.f;
	
	UPROPERTY(EditDefaultsOnly)
	float DebuffFrequency = 0.f;
	
	UPROPERTY(EditDefaultsOnly)
	bool bPitchOverride = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float Pitch = 0.f;
	
};
class UAnimMontage;

/**
 * 
 */
class INVENTORYPROJECT_API IEnemyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCombatTarget(AActor* InCombatTarget);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetCombatTarget() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<UAnimMontage*> GetAttackMontages();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetDeathMontage();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FProjectileInfo GetProjectileInfo() const;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	
};
