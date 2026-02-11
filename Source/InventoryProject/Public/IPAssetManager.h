// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "IPAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYPROJECT_API UIPAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	static UIPAssetManager& Get();

protected:

	virtual void StartInitialLoading() override;
};
