// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemInfo.generated.h"

/**
 * 
 */


class UGameplayEffect;

USTRUCT(BlueprintType)
struct FIPItemInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ItemID = TEXT("None");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* ItemIcon = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Description = TEXT("");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 StackSize = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ItemEffect;
	
	// UPROPERTY(BlueprintReadOnly)
	// int32 Index = -1;

	bool IsValid() const
	{
		if (ItemID == TEXT("None"))
			return false;
		return true;
	}
	
};


UCLASS()
class INVENTORYPROJECT_API UItemInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FIPItemInfo FindItemInfoForName(const FName& ItemID) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<FIPItemInfo> ItemInformation;

	
};
