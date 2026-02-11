// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/IPDungeonGameMode.h"

#include "NavigationSystem.h"
#include "Blueprint/UserWidget.h"
#include "Characters/IPBossCharacter.h"
#include "Characters/IPEnemyCharacter.h"
#include "Engine/AssetManager.h"
#include "Engine/TargetPoint.h"
#include "Interfaces/BossInterface.h"
#include "Kismet/GameplayStatics.h"
#include "SavePoint/IPSaveGate.h"


AIPDungeonGameMode::AIPDungeonGameMode()
{
	PrimaryActorTick.bCanEverTick=true;
}

void AIPDungeonGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	TimePassed+=DeltaSeconds;
	if (CountDown<InitTime && (CachedInitTime-CountDown)>=InitTime-TimePassed)
	{
		CountDownSecondDelegate.Broadcast(CachedInitTime-CountDown);
		CountDown++;
	}
	if (InitTime-TimePassed<=0 && CountdownWidget)
	{
		CountdownWidget->RemoveFromParent();
	}

	if(!bDidStageStart && TimePassed>=InitTime)
	{
		bDidStageStart=true;
		if (GetCurrentStageSpawnerTableRow()->TotalEnemyToSpawnThisStage ==0)
		{
			SaveGate = Cast<AIPSaveGate>(UGameplayStatics::GetActorOfClass(this, AIPSaveGate::StaticClass()));
			SaveGate->EnableCollision(true);
		}
		else
		{
			CurrentSpawnedEnemiesCounter += TrySpawnEnemies();
		}
		
	}
}

void AIPDungeonGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	PreLoadEnemies();
	CachedInitTime=InitTime;
	if (UWorld* World = GetWorld())
	{
		if (CountdownWidget == nullptr)
		{
			CountdownWidget = CreateWidget<UUserWidget>(World, CountdownWidgetClass);
		}
		CountdownWidget->AddToViewport();
	}
}

void AIPDungeonGameMode::PreLoadEnemies()
{
	
	PreLoadedEnemyClassMap.Empty(); //혹시 모르니 아예 비워놓고 시작

	for (const FIPEnemySpawnerInfo& SpawnerInfo : GetCurrentStageSpawnerTableRow()->EnemySpawnerDefinitions)
	{
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;
		
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SpawnerInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda(
				[SpawnerInfo, this]()
				{
					if (UClass* LoadedEnemyClass = SpawnerInfo.SoftEnemyClassToSpawn.Get())
					{
						PreLoadedEnemyClassMap.Emplace(SpawnerInfo.SoftEnemyClassToSpawn, LoadedEnemyClass);
						EnemyTypesCount++;
						//로딩 될 에너미의 종류와 로딩된 에너미의 종류 개수가 같아지면 게임 시작
						bDidStageStart = !(EnemyTypesCount==GetCurrentStageSpawnerTableRow()->EnemySpawnerDefinitions.Num());
						// if (!bDidStageStart)
						// {
						// 	UE_LOG(LogTemp, Warning, TEXT("Enemy Map Size: %d"),PreLoadedEnemyClassMap.Num());
						// }
					}
				}
			)
		);
	}
}

FIPEnemySpawnerTableRow* AIPDungeonGameMode::GetCurrentStageSpawnerTableRow() const
{
	FString WorldName = GetWorld()->GetMapName();
	WorldName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	const FName RowName = FName(GetMapNameFromMapAssetName(WorldName));
	//UE_LOG(LogTemp, Warning,TEXT("MapName is %s"), (*RowName.ToString()));
	FIPEnemySpawnerTableRow* FoundRow = EnemySpawnerDataTable->FindRow<FIPEnemySpawnerTableRow>(RowName, FString());
	
	checkf(FoundRow,TEXT("Could Not Find a Valid Row Under The Name %s in the Data Table"),*RowName.ToString());

	return FoundRow;
}

int32 AIPDungeonGameMode::TrySpawnEnemies()
{
	if (TargetPointsArray.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPointsArray);
	}
	checkf(!TargetPointsArray.IsEmpty(), TEXT("No Valid TargetPoint Found in Level: %s for Spawning Enemies"),*GetWorld()->GetName());
	
	int32 EnemiesSpawnedThisTime = 0;
	
	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (const FIPEnemySpawnerInfo& SpawnerInfo : GetCurrentStageSpawnerTableRow()->EnemySpawnerDefinitions)
	{
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		const int32 NumToSpawn = FMath::RandRange(SpawnerInfo.MinPerSpawnCount, SpawnerInfo.MaxPerSpawnCount); 
		UClass* LoadedEnemyClass = PreLoadedEnemyClassMap.FindChecked(SpawnerInfo.SoftEnemyClassToSpawn);
		
		for (int32 i = 0; i < NumToSpawn; i++)
		{
			const int32 RandomTargetPointIndex = FMath::RandRange(0,TargetPointsArray.Num()-1);
			const FVector SpawnOrigin = TargetPointsArray[RandomTargetPointIndex]->GetActorLocation();
			const FRotator SpawnRotation = TargetPointsArray[RandomTargetPointIndex]->GetActorForwardVector().ToOrientationRotator();
			
			FVector RandomLocation;//아래 함수로 참조받아서 값 받는 변수
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, SpawnOrigin, RandomLocation, 50.f);

			RandomLocation += FVector(0.f, 0.f, 150.f);
			
			// AIPCharacterBase* SpawnedEnemy = GetWorld()->SpawnActor<AIPCharacterBase>(LoadedEnemyClass, RandomLocation, SpawnRotation, SpawnParam);

			FTransform SpawnTransform;
			SpawnTransform.SetLocation(RandomLocation);
			SpawnTransform.SetRotation(SpawnRotation.Quaternion());
			
			AIPCharacterBase* SpawnedEnemy = GetWorld()->SpawnActorDeferred<AIPCharacterBase>(LoadedEnemyClass,SpawnTransform);
			
			if (SpawnedEnemy->Implements<UBossInterface>())
			{
				bIsBossFighting = true;
				SpawnedEnemy = Cast<AIPBossCharacter>(SpawnedEnemy); 
			}
			
			else 
            {
            	SpawnedEnemy = Cast<AIPEnemyCharacter>(SpawnedEnemy); 
            }

			SpawnedEnemy->FinishSpawning(SpawnTransform);
			
			if (SpawnedEnemy)
			{
				SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this,&ThisClass::OnEnemyDestroyed);
				SpawnedEnemy->SpawnDefaultController();
				EnemiesSpawnedThisTime++;
				TotalSpawnedEnemiesThisStageCounter++;
			}

			if (!ShouldKeepSpawnEnemies())
			{
				return EnemiesSpawnedThisTime;
			}
		}
	}
	return EnemiesSpawnedThisTime;
}

bool AIPDungeonGameMode::ShouldKeepSpawnEnemies() const
{
	return TotalSpawnedEnemiesThisStageCounter < GetCurrentStageSpawnerTableRow()->TotalEnemyToSpawnThisStage;
}

void AIPDungeonGameMode::OnEnemyDestroyed(AActor* DestroyedActor)
{
	CurrentSpawnedEnemiesCounter--;
	if (ShouldKeepSpawnEnemies())
	{
		CurrentSpawnedEnemiesCounter += TrySpawnEnemies();
	}
	else if (CurrentSpawnedEnemiesCounter <= 0)
	{
		if (bIsBossFighting)
		{
			if (UWorld* World = GetWorld())
			{
				if (NoticeWidget == nullptr)
				{
					NoticeWidget = CreateWidget<UUserWidget>(World, NoticeWidgetClass);
				}
				NoticeWidget->AddToViewport();
			}
		}
		SaveGate = Cast<AIPSaveGate>(UGameplayStatics::GetActorOfClass(this, AIPSaveGate::StaticClass()));
		SaveGate->EnableCollision(true);
	}
}

void AIPDungeonGameMode::RegisterSpawnedEnemies(const TArray<AIPEnemyCharacter*>& InEnemiesToRegister)
{
	for (AIPEnemyCharacter* SpawnedEnemy : InEnemiesToRegister)
	{
		if (SpawnedEnemy)
		{
			CurrentSpawnedEnemiesCounter++;
			SpawnedEnemy->SpawnDefaultController();
			SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);//보스가 따로 스폰한 적들에 콜백함수 등록
		}
	}
}

