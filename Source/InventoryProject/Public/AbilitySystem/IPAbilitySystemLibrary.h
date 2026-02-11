// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IPAbilitySystemLibrary.generated.h"

class UAbilityInfo;
/**
 * 
 */
class AIPHUD;
class UOverlayWidgetController;
class UAttributeMenuWidgetController;
class UIPInventoryWidgetController;
class UCharacterClassInfo;
class UBossCharacterInfo;
class UItemInfo;
class ULootInfo;

struct FWidgetControllerParams;

UCLASS()
class INVENTORYPROJECT_API UIPAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/*
	 * Widget Controller
	 */

	UFUNCTION(BlueprintPure, Category="IPAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AIPHUD*& OutIPHUD);
	
	UFUNCTION(BlueprintPure, Category="IPAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="IPAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="IPAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static USkillMenuWidgetController* GetSkillMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "IPAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UIPInventoryWidgetController* GetInventoryWidgetController(const UObject* WorldContextObject);

	/*
	* Ability System Class Defaults
	*/

	UFUNCTION(BlueprintCallable, Category="IPAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category="IPAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributesForBoss(const UObject* WorldContextObject, EBossClass BossClass, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category="IPAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributesFromSaveData(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ULoadScreenSaveGame* SaveGame);
	
	UFUNCTION(BlueprintCallable, Category="IPAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category="IPAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilitiesForBoss(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, EBossClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category="IPAbilitySystemLibrary|CharacterClassDefaults")
	static UItemInfo* GetItemInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="IPAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="IPAbilitySystemLibrary|CharacterClassDefaults")
	static UBossCharacterInfo* GetBossCharacterInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="IPAbilitySystemLibrary|CharacterClassDefaults")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="IPAbilitySystemLibrary|CharacterClassDefaults")
	static ULootInfo* GetLootInfo(const UObject* WorldContextObject);
	


		/*
	 * Effect Context Getters
	 */

	UFUNCTION(BlueprintPure, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static bool IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static FVector GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static FVector GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	/*
	 * Effect Context Setters
	 */

	UFUNCTION(BlueprintCallable, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);
	
	UFUNCTION(BlueprintCallable, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static void SetIsSuccessfulDebuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInSuccessfulDebuff);

	UFUNCTION(BlueprintCallable, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDamage);

	UFUNCTION(BlueprintCallable, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffDuration(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDuration);

	UFUNCTION(BlueprintCallable, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffFrequency(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InFrequency);

	UFUNCTION(BlueprintCallable, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static void SetDamageType(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType);

	UFUNCTION(BlueprintCallable, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InImpulse);

	UFUNCTION(BlueprintCallable, Category = "IPAbilitySystemLibrary|GameplayEffects")
	static void SetKnockbackForce(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InForce);


	/*
	 * Gameplay Mechanics
	 */
	static int32 GetXPRewardForLevel(const UObject* WorldContextObject, int32 CharacterLevel);
	static int32 GetXPRewardOfBoss(const UObject* WorldContextObject,EBossClass CharacterClass);

	UFUNCTION(BlueprintPure, Category = "IPAbilitySystemLibrary|GameplayMechanics")
	static bool IsHostile(AActor* FirstActor, AActor* SecondActor);

	UFUNCTION(BlueprintCallable, Category = "IPAbilitySystemLibrary|Gameplay Mechanics")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);
	
	UFUNCTION(BlueprintPure, Category = "IPAbilitySystemLibrary|Gameplay Mechanics")
	static FGameplayTag ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference);

	UFUNCTION(BlueprintPure, Category = "IPAbilitySystemLibrary|GameplayMechanics")
	static TArray<FVector> EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors, float MinDistance, float MaxDistance);
};



