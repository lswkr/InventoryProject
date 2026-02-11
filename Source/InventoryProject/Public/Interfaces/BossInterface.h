// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/BossCharacterInfo.h"
#include "UObject/Interface.h"
#include "BossInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBossInterface : public UInterface
{
	GENERATED_BODY()
};

class AIPEnemyCharacter;

USTRUCT(BlueprintType)
struct FSummonEnemyInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<AIPEnemyCharacter> EnemyClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 NumToSpawn = 1;
};


/**
 * 
 */
class INVENTORYPROJECT_API IBossInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EBossClass GetBossClass();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<UAnimMontage*> GetMeleeAttackMontages();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<UAnimMontage*> GetRangeAttackMontages();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetSummonMontage();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FSummonEnemyInfo GetSummonEnemyInfo();
};
