// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetControllers/SkillMenuWidgetController.h"

#include "IPGameplayTags.h"
#include "NativeGameplayTags.h"
#include "AbilitySystem/IPAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Interfaces/CombatInterface.h"
#include "Player/IPPlayerState.h"

void USkillMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	//UE_LOG(LogTemp, Log, TEXT("BroadcastInitialValues in USkillMenuWidgetController"));
	AActor* AvatarActor = GetIPASC()->GetAvatarActor();
	
	SkillPointsChanged.Broadcast(GetIPPS()->GetSkillPoints());
}

void USkillMenuWidgetController::BindCallbacksToDependencies()
{
	//한 SkillMenuRow마다 바인드
	GetIPASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
	{
		/*
		 * Status가 Eligible이면 Button켜고 옮기기 불가.
		 * Unlock||Equipped면 Button켜고 옮기기 가능
		 */
		//UE_LOG(LogTemp, Warning, TEXT("AbilityStatusChanged. SkillPoints %d left"), GetIPPS()->GetSkillPoints());
		FIPGameplayTags GameplayTags = FIPGameplayTags::Get();
		bool bSkillPointMoreThanZero = GetIPPS()->GetSkillPoints() > 0;
		
		if (StatusTag == GameplayTags.Abilities_Status_Eligible)
		{
			SkillRowDelegate.Broadcast(AbilityTag, bSkillPointMoreThanZero && true, false, 0);
		}
		else if (StatusTag == GameplayTags.Abilities_Status_Equipped||StatusTag == GameplayTags.Abilities_Status_Unlocked)
		{
			SkillRowDelegate.Broadcast(AbilityTag, bSkillPointMoreThanZero && true, true, NewLevel);
		}
		else
		{
			SkillRowDelegate.Broadcast(AbilityTag, false, false, 0);
		}
		if (AbilityInfo)
		{
			FIPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	}
	);
	
	if (GetIPASC())
	{
		GetIPASC()->AbilityEquipped.AddUObject(this,&USkillMenuWidgetController::OnAbilityEquipped);
		
		if (GetIPASC()->bStartupAbilitiesGiven) //bStartupAbilitiesGiven는 캐릭터의 ASC에 어빌리티가 주어졌을 때 true가 됨
		{
			BroadcastAbilityInfo(); //직접 호출해서 ASC에 위젯 바인딩 시키면 됨.
		}
		else //안 주어졌을 경우 델리게이트에 바인딩하고, 그 뒤에 AddCharacterAbilities안에서 해당 델리게이트에 브로드캐스팅하면서 호출하게 됨
		{
			GetIPASC()->AbilitiesGivenDelegate.AddUObject(this, &USkillMenuWidgetController::BroadcastAbilityInfo);
		}

	}
	
}

void USkillMenuWidgetController::UpgradeSkill(const FGameplayTag& SkillTag)
{
	if (GetIPASC())
	{
		GetIPASC()->SpendSkillPoint(SkillTag);
		SkillPointsChanged.Broadcast(GetIPPS()->GetSkillPoints());
	}
}

void USkillMenuWidgetController::OnDroppedOnSkillQuickSlot(const FGameplayTag& AbilityTag, const FGameplayTag& QuickSlotTag)
{
	GetIPASC()->EquipAbility(AbilityTag, QuickSlotTag);
}

void USkillMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	const FIPGameplayTags& GameplayTags = FIPGameplayTags::Get();
	
	FIPAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;

	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FIPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);
}


