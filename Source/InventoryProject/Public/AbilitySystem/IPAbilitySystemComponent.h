// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "IPAbilitySystemComponent.generated.h"

/**
 * 
 */
class ULoadScreenSaveGame;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/, int32 /*AbilityLevel*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*Status*/, const FGameplayTag& /*Slot*/, const FGameplayTag& /*PrevSlot*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*AbilityTag*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveEffect, const FGameplayTag& /*AbilityTag*/, bool /*bActivate*/);

UCLASS()
class INVENTORYPROJECT_API UIPAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;
 	FAbilitiesGiven AbilitiesGivenDelegate;
 	FAbilityStatusChanged AbilityStatusChanged;
 	FAbilityEquipped AbilityEquipped;

	void AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveData);
 	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	void AddCharacterAbilities_forBoss(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
 	bool bStartupAbilitiesGiven = false;

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
 	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetSlotFromAbilityTag(const FGameplayTag& AbilityTag);
	bool SlotIsEmpty(const FGameplayTag& Slot);
	static bool DoesAbilityHaveSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);
	static bool DoesAbilityHaveAnySlot(const FGameplayAbilitySpec& Spec);
	FGameplayAbilitySpec* GetSpecWithSlot(const FGameplayTag& Slot);
	static void AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);

	UFUNCTION()
 	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	
	void UpdateAbilityStatuses(int32 Level);

	UFUNCTION()
	void SpendSkillPoint(const FGameplayTag& AbilityTag);

	
	UFUNCTION()
	void EquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Slot);

	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription);

 	static void ClearSlot(FGameplayAbilitySpec* Spec);
 	void ClearAbilitiesOfSlot(const FGameplayTag& Slot);
 	static bool DoesAbilityHaveSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot);
	bool bIsAbilityQualified(const FGameplayTag& AbilityTag);
	
protected:
	UFUNCTION() //EffectAssetTags를 Broadcast하는 함수. OverlayWidgetController에서 메시지 위젯 출력하도록 한다.
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

private:
	UPROPERTY()
	TMap<FGameplayTag, int32> CurrentAbilitiesState;//어빌리티의 스킬 포인트 수 체크하는 맵
	
};
