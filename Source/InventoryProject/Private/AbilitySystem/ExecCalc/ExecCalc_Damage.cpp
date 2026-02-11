// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "IPGameplayTags.h"
#include "AbilitySystem/IPAbilitySystemLibrary.h"
#include "AbilitySystem/IPAttributeSet.h"
#include "Interfaces/BossInterface.h"
#include "Interfaces/CombatInterface.h"
#include "Kismet/GameplayStatics.h"

struct IPDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Attack);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);

	IPDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UIPAttributeSet, Attack, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UIPAttributeSet, Defense, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UIPAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UIPAttributeSet, CriticalHitChance, Source, false);
	}
};

static const IPDamageStatics& DamageStatics()
{
	static IPDamageStatics DStatics;
	return DStatics;
}
UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters EvaluationParameters,
	const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToDefs) const
{
	const FIPGameplayTags& GameplayTags = FIPGameplayTags::Get();

	for (TTuple<FGameplayTag, FGameplayTag> Pair : GameplayTags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageType = Pair.Key;
		const FGameplayTag& DebuffType = Pair.Value;
		
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);

		if (TypeDamage > -.5f) // .5 padding for floating point [im]precision
		{
			// Determine if there was a successful debuff
			float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.f);

			UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
			AActor* TargetAvatar = ExecutionParams.GetTargetAbilitySystemComponent()? TargetASC->GetAvatarActor() : nullptr;

			if (TargetAvatar->Implements<UBossInterface>())
			{
				SourceDebuffChance/=2;	
			}

			const bool bDebuff = FMath::RandRange(1, 100) < SourceDebuffChance;

			
			if (bDebuff)
			{
				//UE_LOG(LogTemp,Warning,TEXT("Debuff:%s "),*TargetAvatar->GetActorNameOrLabel());
				
				FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

				UIPAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
				UIPAbilitySystemLibrary::SetDamageType(ContextHandle, DamageType);

				const float DebuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage, true, -1.f);
				const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, true, -1.f);
				const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, true, -1.f);

				UIPAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
				UIPAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
				UIPAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
			}
		}
	}
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	const FIPGameplayTags& Tags = FIPGameplayTags::Get();
	
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, DamageStatics().BlockChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Attack, DamageStatics().AttackDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Defense, DamageStatics().DefenseDef);
	
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	
	// Debuff
	DetermineDebuff(ExecutionParams, Spec, EvaluationParameters, TagsToCaptureDefs);

	// Get Damage Set by Caller Magnitude
	float Damage = 1.f;
	
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair  : Tags.DamageTypesToDebuffs)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);

		if (DamageTypeValue<=0.f)
		{
			continue;
		}
		Damage += DamageTypeValue;
	}
	float TargetDefense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluationParameters, TargetDefense);
	TargetDefense = FMath::Max<float>(TargetDefense, 0.f);
	
	float SourceAttack = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackDef, EvaluationParameters, SourceAttack);
	SourceAttack = FMath::Max<float>(SourceAttack, 0.f);
	
	Damage *= ( 100 - TargetDefense + 2 * SourceAttack) / 100.f;

	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);
	
	const float EffectiveCriticalHitChance = SourceCriticalHitChance;
	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;
	
	UIPAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	// Double damage
	Damage = bCriticalHit ? 2.f * Damage : Damage;

	float TargetBlockChance = 1.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance *= FMath::Max(0, (TargetPlayerLevel-SourcePlayerLevel)/100);//Level차이. TargetLevel이 높을수록 더 높은 Block확률(TargetLevel-Source)/100
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	
	UIPAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	
	const FGameplayModifierEvaluatedData EvaluatedData(UIPAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
