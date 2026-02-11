// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootInfo.generated.h"

/**
 * 
 */

class AIPItemBase;

USTRUCT(BlueprintType)
struct FLootSpawnInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 AllowingEnemyLevel = 1;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 AllowingLevelInterval = 1;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float LootSpawnChance = 20.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<AIPItemBase> ItemClass;
};

UCLASS()
class INVENTORYPROJECT_API ULootInfo : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FLootSpawnInfo> LootInformation;

public:
	TArray<FLootSpawnInfo> GetLootInfoBasedOnLevel(const int32& EnemyLevel);
};
