// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/BossCharacterInfo.h"

FBossClassDefaultInfo UBossCharacterInfo::GetBossClassDefaultInfo(EBossClass CharacterClass)
{
	return BossClassInformation.FindChecked(CharacterClass);
}
