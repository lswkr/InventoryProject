// Fill out your copyright notice in the Description page of Project Settings.


#include "IPAssetManager.h"

#include "IPGameplayTags.h"

UIPAssetManager& UIPAssetManager::Get()
{
	check(GEngine);
	
	UIPAssetManager* IPAssetManager = Cast<UIPAssetManager>(GEngine->AssetManager);
	return *IPAssetManager;
}

void UIPAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FIPGameplayTags::InitializeNativeGameplayTags();
}
