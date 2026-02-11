// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BossCharacterInfo.generated.h"

/**
 * 
 */
class UGameplayEffect;
class UGameplayAbility;


UENUM(BlueprintType)
enum class EBossClass : uint8
{
	Grux
};

USTRUCT(BlueprintType)
struct FBossClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	int XPReward = 0;
};

UCLASS(Blueprintable)
class INVENTORYPROJECT_API UBossCharacterInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TMap<EBossClass,FBossClassDefaultInfo> BossClassInformation;
	
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;
	
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults") //Death, HitReact등등
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	FBossClassDefaultInfo GetBossClassDefaultInfo(EBossClass CharacterClass);
};
