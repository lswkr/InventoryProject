// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IPGameModeBase.generated.h"

class UMVVM_LoadSlot;
class ULoadScreenSaveGame;
class USaveGame;
class UAbilityInfo;
class UCharacterClassInfo;
class UBossCharacterInfo;
class UItemInfo;
/**
 * 
 */



UCLASS()
class INVENTORYPROJECT_API AIPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	UCharacterClassInfo* CharacterClassInfo;

	// UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	// UBossCharacterInfo* BossCharacterInfo;

	UPROPERTY(EditDefaultsOnly)
	UAbilityInfo* AbilityInfo;

	UPROPERTY(EditDefaultsOnly)
	UItemInfo* ItemInfo;

	UPROPERTY(EditAnywhere)
	int32 StageNum = 1;
	
	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex);
	ULoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);
	ULoadScreenSaveGame* RetrieveInGameSaveData();
	void SaveInGameProgressData(ULoadScreenSaveGame* SaveObject);

	void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName = FString("")) const;
	void LoadWorldState(UWorld* World) const;
	
	void TravelToMap(UMVVM_LoadSlot* Slot);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;
		
	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;

	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	void PlayerDied(ACharacter* DeadCharacter);

protected:
	virtual void BeginPlay() override;


};
