// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP) const
{
	int32 Level = 1;
	bool bSearching = true;
	
	// LevelUpInformation[1] = Level 1 Information(One-indexed)
	
	while (bSearching)
	{
		if (LevelUpInformation.Num() - 1 <= Level) return Level;//레벨 정보배열보다 레벨이 높지 않도록

		//XP --------------------------------------------------
		//                      | <- (Level x에 해당하는 XP조건)
		//XP의 총합을 두고 Level의 정보에 저장된 요구 XP를 보며 그 길이에 맞는 레벨을 찾는 방식
		if (XP >= LevelUpInformation[Level].LevelUpRequirement)
		{
			Level++;
		}
		else
		{
			bSearching = false;
		}
	}
	return Level;
}
