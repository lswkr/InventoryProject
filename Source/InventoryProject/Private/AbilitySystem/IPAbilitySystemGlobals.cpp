// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/IPAbilitySystemGlobals.h"

#include "IPAbilityTypes.h"

FGameplayEffectContext* UIPAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FIPGameplayEffectContext();
}
