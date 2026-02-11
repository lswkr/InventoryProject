// Fill out your copyright notice in the Description page of Project Settings.


#include "IPGameplayTags.h"

#include "GameplayTagsManager.h"

FIPGameplayTags FIPGameplayTags::GameplayTags;

void FIPGameplayTags::InitializeNativeGameplayTags()
{
	/* Input Tags */
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.1"),
		FString("Input Tag for key 1")
		);

	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.2"),
		FString("Input Tag for key 2")
		);

	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.3"),
		FString("Input Tag for key 3")
		);

	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.4"),
		FString("Input Tag for key 4")
		);

	GameplayTags.InputTag_Z = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Z"),
		FString("Input Tag for key Z")
		);

	GameplayTags.InputTag_X = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.X"),
		FString("Input Tag for key X")
		);

	GameplayTags.InputTag_C = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.C"),
		FString("Input Tag for key C")
		);

	GameplayTags.InputTag_V = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.V"),
		FString("Input Tag for key V")
		);

	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InputTag.LMB"),
			FString("Input Tag for key Left Mouse Button")
			);
	
	/* Player */
	GameplayTags.Player_Block_InputPressed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputPressed"),
		FString("Status that player cannot press input")
		);

	GameplayTags.Player_Block_InputReleased = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputReleased"),
		FString("Status that player cannot release input")
		);

	GameplayTags.Player_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputHeld"),
		FString("Status that player cannot hold input")
		);

	
	/* Primary Attributes */
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("Increase MaxHP and Attack Power")
		);
	
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("Increase MaxMP and Magic Power")
		);
	
	GameplayTags.Attributes_Primary_Luck = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Luck"),
		FString("Increase Critical Hit Chance and Block Chance")
		);

	GameplayTags.Attributes_Primary_Will = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Will"),
		FString("Increase Defense Power and Amount of Health/Mana Regenerated")
		);

	

	/* Secondary Attributes */
	GameplayTags.Attributes_Secondary_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.Attack"),
	FString("Attack Power")
	);
	
	GameplayTags.Attributes_Secondary_Defense = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.Defense"),
	FString("Defense Power")
	);

	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.CriticalHitChance"),
	FString("Chance to Increase Damage")
	);

	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.BlockChance"),
	FString("Chance to Block")
	);

	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.HealthRegeneration"),
	FString("Amount of Health Regenerated Every 1 Second")
	);

	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.ManaRegeneration"),
	FString("Amount of Mana Regenerated Every 1 Second")
	);

	
	/* Vital Attributes */
	GameplayTags.Attributes_Vital_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Vital.MaxHealth"),
	FString("Maximum Amount of Health Obtainable")
	);

	GameplayTags.Attributes_Vital_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Vital.MaxMana"),
	FString("Maximum Amount of Mana Obtainable")
	);

	GameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Vital.Health"),
	FString("Amount of Current Health")
	);

	GameplayTags.Attributes_Vital_Mana = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Vital.Mana"),
	FString("Amount of Current Mana")
	);

	/* Meta Attributes */
	GameplayTags.Attributes_Meta_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Meta.IncomingXP"),
	FString("IncomingXP Meta Attributes")
	);

	GameplayTags.Attributes_Meta_IncomingDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Meta.IncomingDamage"),
	FString("IncomingDamage Meta Attributes")
	);

	/* Character Statuses */
	GameplayTags.Status_Dead = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Status.Dead"),
	FString("Dead Status")
	);

	/* Ability Status */
	GameplayTags.Abilities_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Equipped"),
		FString("Equipped Status")
		);

	GameplayTags.Abilities_Status_Unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Unlocked"),
		FString("Unlocked Status")
		);
	
	GameplayTags.Abilities_Status_Eligible = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Eligible"),
		FString("Eligible Status")
		);
	
	GameplayTags.Abilities_Status_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Locked"),
		FString("Locked Status")
		);
	
	
	/* Abilities */
	GameplayTags.Abilities_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.None"),
		FString("No Ability")
		);

	GameplayTags.Abilities_DefaultAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.DefaultAttack"),
		FString("Default Attack")
		);
	
	GameplayTags.Abilities_Passive_ListenForEvent = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Passive.ListenForEvent"),
		FString("Listen for event")
		);
	
	GameplayTags.Abilities_Skill_Shoryuken = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Abilities.Skill.Shoryuken"),
			FString("Skill: Shoryuken")
			);

	GameplayTags.Abilities_Skill_GearThrow = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Abilities.Skill.GearThrow"),
			FString("Skill: GearThrow")
			);

	GameplayTags.Abilities_Skill_Explosion = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Abilities.Skill.Explosion"),
			FString("Skill: Explosion")
			);

	GameplayTags.Abilities_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Abilities.HitReact"),
			FString("HitReact Ability")
			);

	GameplayTags.Abilities_Death = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Death"),
		FString("Death Ability")
		);


	GameplayTags.Abilities_Enemy_Attack_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Abilities.Enemy.Attack.Melee"),
			FString("Melee Attack Ability for Enemy")
			);

	GameplayTags.Abilities_Enemy_Attack_Range = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Enemy.Attack.Range"),
		FString("Range Attack Ability for Enemy")
		);

	GameplayTags.Abilities_Enemy_SummonEnemies = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Abilities.Enemy.SummonEnemies"),
			FString("SummonEnemies Ability for Enemy")
			);

	GameplayTags.Abilities_Player_HitPause = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Abilities.Player.HitPause"),
			FString("HitPause Ability")
			);

	/* Cooldown Tags */
	GameplayTags.Cooldown_Shoryuken = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Cooldown.Shoryuken"),
			FString("Shoryuken Cooldown Tag")
			);
	
	GameplayTags.Cooldown_Spear = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Cooldown.Spear"),
			FString("Spear Cooldown Tag")
			);

	GameplayTags.Cooldown_Explosion = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Cooldown.Explosion"),
			FString("Explosion Cooldown Tag")
			);

	GameplayTags.Cooldown_GearThrow= UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Cooldown.GearThrow"),
			FString("GearThrow Cooldown Tag")
			);
	
	/* Type */
	GameplayTags.Abilities_Type_Active = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Type.Active"),
	FString("Tag for Active Abilities")
	);

	GameplayTags.Abilities_Type_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Type.Passive"),
FString("Tag for Passive Abilities")
	);
	
	/* Damage Type */
	GameplayTags.DamageType_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("DamageType.Physical"),
		FString("DamageType that occurs Stun")
		);

	GameplayTags.DamageType_Bleed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("DamageType.Bleed"),
	FString("DamageType that occurs Bleeding")
	);
	
	/* Debuff Type */
	GameplayTags.Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Stun"),
		FString("Debuff that makes character can't move for a few seconds")
		);

	GameplayTags.Debuff_Bleeding = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Debuff.Bleeding"),
	FString("Debuff that gives damage for a few seconds")
	);

	/* Damage Type */
	GameplayTags.Abilities_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.None"),
		FString("No Ability")
		);
	
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.DamageType_Physical, GameplayTags.Debuff_Stun);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.DamageType_Bleed, GameplayTags.Debuff_Bleeding);

	/* Debuff Value */
	GameplayTags.Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Debuff.Chance"),
	FString("Debuff Chance")
	);

	
	GameplayTags.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Debuff.Duration"),
	FString("Debuff Duration")
	);

	
	GameplayTags.Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Debuff.Damage"),
	FString("Debuff Damage")
	);

	
	GameplayTags.Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Debuff.Frequency"),
	FString("Debuff Frequency")
	);
	
	/* Events */
	GameplayTags.Event_Shared_Hit = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Event.Shared.Hit"),
	FString("Tag that transferred when hit")
	);
	
	GameplayTags.Event_Player_HitPause = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Event.Player.HitPause"),
	FString("Tag that transferred when hit, causing hit pause effect")
	);
	
	GameplayTags.Event_Shared_SpawnProjectile = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Event.Shared.SpawnProjectile"),
	FString("Tag that transferred when Spawn Projectile")
	);
	
	GameplayTags.Event_Shared_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Event.Shared.HitReact"),
	FString("Tag that transferred to occur HitReact of Target")
	);

	GameplayTags.Event_Enemy_SummonEnemies = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Event.Enemy.SummonEnemies"),
	FString("Tag that transferred to summon enemies")
	);

	/* Effects */
	GameplayTags.Effect_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Effect.HitReact"),
	FString("Tag that granted when enemy hit")
	);

	/* Hit Direction */
	GameplayTags.Player_HitReact_Front = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Player.HitReact.Front"),
	FString("Player HitReact Tag when attack comes to front of player")
	);

	GameplayTags.Player_HitReact_Back = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Player.HitReact.Back"),
	FString("Player HitReact Tag when attack comes to back of player")
	);

	GameplayTags.Player_HitReact_Front = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Player.HitReact.Left"),
	FString("Player HitReact Tag when attack comes to left of player")
	);

	GameplayTags.Player_HitReact_Front = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Player.HitReact.Right"),
	FString("Player HitReact Tag when attack comes to right of player")
	);
}
