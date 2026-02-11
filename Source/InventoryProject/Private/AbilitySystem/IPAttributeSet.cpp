// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/IPAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "IPGameplayTags.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "GameplayEffectExtension.h"
#include "IPAbilityTypes.h"
#include "AbilitySystem/IPAbilitySystemComponent.h"
#include "AbilitySystem/IPAbilitySystemLibrary.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Interfaces/BossInterface.h"
#include "Interfaces/EnemyInterface.h"
#include "Interfaces/PlayerInterface.h"
#include "Logging/StructuredLog.h"
#include "Player/IPPlayerController.h"

UIPAttributeSet::UIPAttributeSet()
{
	const FIPGameplayTags& GameplayTags = FIPGameplayTags::Get();

	/* Primary Attributes */
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Luck, GetLuckAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Will, GetWillAttribute);
	
	/* Secondary Attributes */
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Attack, GetAttackAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Defense, GetDefenseAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	
	/* Vital Attributes */
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_MaxMana, GetMaxManaAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Health, GetHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Mana, GetManaAttribute);
}

void UIPAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	
}

void UIPAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	if(Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter)) return;

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(Props);
	}
	
}

void UIPAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth) //레벨업 시 MaxHealth의 값이 바뀌므로 이를 Health값에 반영
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
	}
	if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}

}

void UIPAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	
	if (LocalIncomingDamage > 0.f)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		const bool bFatal = NewHealth <= 0.f;
		if (bFatal) //죽을 때
		{
			ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
			if (CombatInterface)
			{
				CombatInterface->Die(UIPAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle)); 
			}
		
			SendXPEvent(Props);
		}
		else
		{	
			if (Props.TargetCharacter->Implements<UPlayerInterface>()) //플레이어는 방향에 따른 애님 몽타주 변경을 위해 Event로
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FIPGameplayTags::Get().Abilities_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);

				FGameplayEventData Data;
				Data.Instigator = Props.SourceAvatarActor;

				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
					Props.TargetASC->GetAvatarActor(),
					FIPGameplayTags::Get().Event_Shared_HitReact,
					Data);
			}
			else //적들은 어빌리티 발동만 하면 되므로 TryActivateAbilitiesByTag
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FIPGameplayTags::Get().Abilities_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}

			const FVector& KnockbackForce = UIPAbilitySystemLibrary::GetKnockbackForce(Props.EffectContextHandle);
			//튕기도록
			if (!KnockbackForce.IsNearlyZero(1.f))
			{
				Props.TargetCharacter->LaunchCharacter(KnockbackForce, true, true);
			}
		}
			
		const bool bBlock = UIPAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);//Miss
		const bool bCriticalHit = UIPAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);//Critical

		ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCriticalHit);
		
		if (UIPAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
		{
			Debuff(Props);
		}
	}	
}

void UIPAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
	const float LocalIncomingXP = GetIncomingXP();
	SetIncomingXP(0.f);

	if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
	{
		const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);

		//UE_LOG(LogTemp, Warning, TEXT("XP Point: %f"), CurrentXP + LocalIncomingXP);
		
		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
		const int32 NumLevelUps = NewLevel - CurrentLevel;//레벨업 안 되면 0
		if (NumLevelUps > 0)
		{
			IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumLevelUps);

			int32 AttributePointsReward = 0;
			int32 SkillPointsReward = 0;

			for (int32 i = 0; i < NumLevelUps; i++)
			{
				AttributePointsReward += IPlayerInterface::Execute_GetAttributePointsReward(Props.SourceCharacter, CurrentLevel + i);
				SkillPointsReward += IPlayerInterface::Execute_GetSkillPointsReward(Props.SourceCharacter, CurrentLevel + i);
			}

			IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
			IPlayerInterface::Execute_AddToSkillPoints(Props.SourceCharacter, SkillPointsReward);
			
			SetHealth(GetMaxHealth());
			SetMana(GetMaxMana());
			bTopOffHealth = true;
			bTopOffMana = true;
			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
		}

		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
	}
}

void UIPAttributeSet::Debuff(const FEffectProperties& Props)
{
	const FIPGameplayTags& GameplayTags = FIPGameplayTags::Get();
	FGameplayEffectContextHandle EffectContext = Props.SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Props.SourceAvatarActor);

	const FGameplayTag DamageType = UIPAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
	const float DebuffDamage = UIPAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffDuration = UIPAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float DebuffFrequency = UIPAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);

//	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage()/*,FName(DebuffName)*/);

	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = DebuffFrequency;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);

	//수정본start//
	
	// FInheritedTagContainer InheritedTagContainer = FInheritedTagContainer();
	// UTargetTagsGameplayEffectComponent& AssetTagsComponent = Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
	// InheritedTagContainer.Added.AddTag(DebuffTag);
	// InheritedTagContainer.CombinedTags.AddTag(DebuffTag);
	// AssetTagsComponent.SetAndApplyTargetTagChanges(InheritedTagContainer);
	//
	// if (DebuffTag.MatchesTagExact(GameplayTags.Debuff_Stun))
	// {
	// 	InheritedTagContainer.Added.AddTag(GameplayTags.Player_Block_InputHeld);
	// 	InheritedTagContainer.Added.AddTag(GameplayTags.Player_Block_InputPressed);
	// 	InheritedTagContainer.Added.AddTag(GameplayTags.Player_Block_InputReleased);
	// }
	// AssetTagsComponent.SetAndApplyTargetTagChanges(InheritedTagContainer);

	FInheritedTagContainer TagContainer = FInheritedTagContainer();
	UTargetTagsGameplayEffectComponent& Component = Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
	TagContainer.Added.AddTag(GameplayTags.DamageTypesToDebuffs[DamageType]);
	// if (Props.TargetAvatarActor->Implements<UPlayerInterface>())
	// {
	// 	TagContainer.Added.AddTag(GameplayTags.Player_Block_InputHeld);
	// 	TagContainer.Added.AddTag(GameplayTags.Player_Block_InputPressed);
	// 	TagContainer.Added.AddTag(GameplayTags.Player_Block_InputReleased);
	// }
	//TagContainer.CombinedTags.AddTag(GameplayTags.DamageTypesToDebuffs[DamageType]);
	Component.SetAndApplyTargetTagChanges(TagContainer);
	//수정본end//

	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	//Effect->StackLimitCount = 1;

	const int32 Index = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];

	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = UIPAttributeSet::GetIncomingDamageAttribute();

	
	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
	{
		FIPGameplayEffectContext* IPContext = static_cast<FIPGameplayEffectContext*>(MutableSpec->GetContext().Get());
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		IPContext->SetDamageType(DebuffDamageType);
		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}

}

void UIPAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UIPAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit,
	bool bCriticalHit) const
{
	if (!IsValid(Props.SourceCharacter) || !IsValid(Props.TargetCharacter)) return;
	
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if(AIPPlayerController* PC = Cast<AIPPlayerController>(Props.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		if(AIPPlayerController* PC = Cast<AIPPlayerController>(Props.TargetCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
		}
	}
}

void UIPAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.TargetCharacter->Implements<UPlayerInterface>()) return;
	
	if (Props.TargetCharacter->Implements<UBossInterface>())
	{
		const EBossClass TargetClass = IBossInterface::Execute_GetBossClass(Props.TargetCharacter);
		const int32 XPReward = UIPAbilitySystemLibrary::GetXPRewardOfBoss(Props.TargetCharacter, TargetClass);

		//UE_LOG(LogTemp, Warning, TEXT("BossXPREWARD: %d"), XPReward);
		
		const FIPGameplayTags& GameplayTags = FIPGameplayTags::Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXP, Payload);
		
	}
	else if (Props.TargetCharacter->Implements<UEnemyInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
		const int32 XPReward = UIPAbilitySystemLibrary::GetXPRewardForLevel(Props.TargetCharacter, TargetLevel);

		//UE_LOG(LogTemp, Warning, TEXT("XPREWARD: %d"), XPReward);
		const FIPGameplayTags& GameplayTags = FIPGameplayTags::Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXP, Payload);
	}
}

