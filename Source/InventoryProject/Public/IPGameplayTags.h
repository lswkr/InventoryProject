// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FIPGameplayTags
{
	
public:
	static const FIPGameplayTags& Get() { return GameplayTags; };
	static void InitializeNativeGameplayTags();

	/* Input Tags */
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_Z;
	FGameplayTag InputTag_X;
	FGameplayTag InputTag_C;
	FGameplayTag InputTag_V;
	FGameplayTag InputTag_LMB;
	
	/* Player */
	FGameplayTag Player_Block_InputPressed;
	FGameplayTag Player_Block_InputReleased;
	FGameplayTag Player_Block_InputHeld;
	
	/* Primary Attributes */
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Luck;
	FGameplayTag Attributes_Primary_Will;

	/* Secondary Attributes */
	FGameplayTag Attributes_Secondary_Attack;
	FGameplayTag Attributes_Secondary_Defense;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;

	/* Vital Attributes */
	FGameplayTag Attributes_Vital_MaxHealth;
	FGameplayTag Attributes_Vital_MaxMana;
	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_Mana;

	/* Meta Attributes */
	FGameplayTag Attributes_Meta_IncomingXP;
	FGameplayTag Attributes_Meta_IncomingDamage;

	/* Character Statuses */
	FGameplayTag Status_Dead;
	
	/* Ability Status */
	FGameplayTag Abilities_Status_Equipped;
	FGameplayTag Abilities_Status_Unlocked;
	FGameplayTag Abilities_Status_Eligible;
	FGameplayTag Abilities_Status_Locked;
	

	/* Abilities */
	FGameplayTag Abilities_None;//IPAbilityTags에서 제외
	FGameplayTag Abilities_DefaultAttack;
	FGameplayTag Abilities_Passive_ListenForEvent;
	FGameplayTag Abilities_Skill_Shoryuken;
	FGameplayTag Abilities_Skill_Spear;
	FGameplayTag Abilities_Skill_GearThrow;
	FGameplayTag Abilities_Skill_Explosion;
	FGameplayTag Abilities_HitReact;
	FGameplayTag Abilities_Death;
	FGameplayTag Abilities_Enemy_Attack_Melee;
	FGameplayTag Abilities_Enemy_Attack_Range;
	FGameplayTag Abilities_Enemy_SummonEnemies;
	FGameplayTag Abilities_Player_HitPause;
	
	TArray<FGameplayTag> IPAbilityTags;

	/* Cooldown Tags */
	FGameplayTag Cooldown_Shoryuken;
	FGameplayTag Cooldown_Spear;
	FGameplayTag Cooldown_Explosion;
	FGameplayTag Cooldown_GearThrow;
	
	/* Type */
	FGameplayTag Abilities_Type_Passive;
	FGameplayTag Abilities_Type_Active;

	/* Damage Type */
	FGameplayTag DamageType_Physical;
	FGameplayTag DamageType_Bleed;
	
	/* Debuff Type */
	FGameplayTag Debuff_Stun;
	FGameplayTag Debuff_Bleeding;
	
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;
	
	/* Debuff Value */
	FGameplayTag Debuff_Chance;
	FGameplayTag Debuff_Duration;
	FGameplayTag Debuff_Damage;
	FGameplayTag Debuff_Frequency;
	
	/* Events */
	FGameplayTag Event_Shared_Hit;
	FGameplayTag Event_Player_HitPause;
	FGameplayTag Event_Shared_SpawnProjectile;
	FGameplayTag Event_Shared_HitReact;
	FGameplayTag Event_Enemy_SummonEnemies;
	
	/* Effects */
	FGameplayTag Effect_HitReact;

	/* Hit Direction */
	FGameplayTag Player_HitReact_Front;
	FGameplayTag Player_HitReact_Back;
	FGameplayTag Player_HitReact_Left;
	FGameplayTag Player_HitReact_Right;
	
private:
	static FIPGameplayTags GameplayTags;
};
