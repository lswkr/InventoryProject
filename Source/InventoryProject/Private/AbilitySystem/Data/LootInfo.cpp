// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LootInfo.h"

TArray<FLootSpawnInfo> ULootInfo::GetLootInfoBasedOnLevel(const int32& EnemyLevel)
{
	TArray<FLootSpawnInfo> OutLootInfo;

	for (const FLootSpawnInfo& Info : LootInformation)
	{
		int MinLootLevel = FMath::Max(Info.AllowingEnemyLevel-Info.AllowingLevelInterval,0);
		int MaxLootLevel = Info.AllowingEnemyLevel+Info.AllowingLevelInterval;

		if (EnemyLevel>=MinLootLevel && EnemyLevel<=MaxLootLevel)
		{
			const bool bSpawnChance = FMath::RandRange(1, 100) < Info.LootSpawnChance;
			if (bSpawnChance)
			{
				OutLootInfo.Add(Info);
			}
	
		}
	}
	return OutLootInfo;
}
