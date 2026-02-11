// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/IPPlayerState.h"

#include "AbilitySystem/IPAbilitySystemComponent.h"
#include "AbilitySystem/IPAttributeSet.h"

AIPPlayerState::AIPPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UIPAbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<UIPAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AIPPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AIPPlayerState::AddToXP(int32 InXP)
{
	XP+=InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AIPPlayerState::AddToLevel(int32 InLevel)
{
	Level+=InLevel;
	OnLevelChangedDelegate.Broadcast(Level, true);
}

void AIPPlayerState::AddToAttributePoints(int32 InPoints)
{
	AttributePoints += InPoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AIPPlayerState::AddToSkillPoints(int32 InPoints)
{
	SkillPoints+=InPoints;
	OnSkillPointsChangedDelegate.Broadcast(SkillPoints);
}

void AIPPlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AIPPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(Level, false);
}

void AIPPlayerState::SetAttributePoints(int32 InPoints)
{
	AttributePoints = InPoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AIPPlayerState::SetSkillPoints(int32 InPoints)
{
	SkillPoints = InPoints;
	OnSkillPointsChangedDelegate.Broadcast(SkillPoints);
}

void AIPPlayerState::SetCharacterName(FString InCharacterName)
{
	CharacterName = InCharacterName;
	OnPlayerNameInitDelegate.Broadcast(CharacterName);
}
