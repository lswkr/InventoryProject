// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "IPInputConfig.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FIPInputAction{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

UCLASS()
class INVENTORYPROJECT_API UIPInputConfig : public UDataAsset
{
	GENERATED_BODY()

	
public:
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FIPInputAction> InventoryInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FIPInputAction> AbilityInputActions;
};
