// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetControllers/IPWidgetController.h"
#include "SkillMenuWidgetController.generated.h"

/**
 * 
 */
class UAbilityInfo;
struct FAuraAttributeInfo;
struct FGameplayTag;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSkillRowDelegate, const FGameplayTag&, SkillTag, bool, bEnabledSpendButton, bool, bCanPreview, int32, Level);


UCLASS(Blueprintable)
class INVENTORYPROJECT_API USkillMenuWidgetController : public UIPWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature SkillPointsChanged;

	UPROPERTY(BlueprintAssignable)
	FSkillRowDelegate SkillRowDelegate;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void UpgradeSkill(const FGameplayTag& SkillTag);

	UFUNCTION(BlueprintCallable)
	void OnDroppedOnSkillQuickSlot(const FGameplayTag& AbilityTag, const FGameplayTag& QuickSlotTag);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);
	
};
