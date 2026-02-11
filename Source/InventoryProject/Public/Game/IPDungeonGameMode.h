// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/IPGameModeBase.h"
#include "IPDungeonGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCountDownDelegate, const int32&, Second);

class AIPSaveGate;
class AIPCharacterBase;
class ULootInfo;

USTRUCT(BlueprintType)
struct FIPEnemySpawnerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AIPCharacterBase> SoftEnemyClassToSpawn;

	UPROPERTY(EditAnywhere)
	int32 MinPerSpawnCount = 1;

	UPROPERTY(EditAnywhere)
	int32 MaxPerSpawnCount = 3;
};

USTRUCT(BlueprintType)
struct FIPEnemySpawnerTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FIPEnemySpawnerInfo> EnemySpawnerDefinitions;

	UPROPERTY(EditAnywhere)
	int32 TotalEnemyToSpawnThisStage = 1;
};

/**
 * 
 */
UCLASS()
class INVENTORYPROJECT_API AIPDungeonGameMode : public AIPGameModeBase
{
	GENERATED_BODY()

public:
	AIPDungeonGameMode();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly)
	ULootInfo* LootInfo;

	//수정
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	UBossCharacterInfo* BossCharacterInfo;

	UPROPERTY(BlueprintAssignable)
	FCountDownDelegate CountDownSecondDelegate;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AIPSaveGate* SaveGate;

	float TimePassed = 0.f;
	bool CheckOnce = false;
	int32 TotalSpawnedEnemiesThisStageCounter=0;
	int32 CurrentSpawnedEnemiesCounter = 0;
	bool bDidStageStart = true;
	bool bDidLoadFinished = false;
	int32 EnemyTypesCount = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WaveDefinition", meta=(AllowPrivateAccess="true"))
	UDataTable* EnemySpawnerDataTable;

	void PreLoadEnemies();
	FIPEnemySpawnerTableRow* GetCurrentStageSpawnerTableRow() const;
	int32 TrySpawnEnemies();
	bool ShouldKeepSpawnEnemies() const;
	
	UFUNCTION()
	void OnEnemyDestroyed(AActor* DestroyedActor);

	UPROPERTY()
	TMap<TSoftClassPtr<AIPCharacterBase>, UClass*> PreLoadedEnemyClassMap;

	UPROPERTY()
	TArray<AActor*> TargetPointsArray;


	UPROPERTY()
	bool bIsBossFighting = false;

	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NoticeWidgetClass;

	UPROPERTY()
	UUserWidget* NoticeWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> CountdownWidgetClass;
	
	UPROPERTY()
	UUserWidget* CountdownWidget;
	
	UPROPERTY(EditDefaultsOnly)
	float InitTime = 5.f;
	int CachedInitTime;
	int CountDown=0;
	
	
public:
	UFUNCTION(BlueprintCallable)
	void RegisterSpawnedEnemies(const TArray<AIPEnemyCharacter*>& InEnemiesToRegister);


	

	
};
