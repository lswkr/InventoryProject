// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/WaitSpawnEnemies.h"

#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
#include "Characters/IPEnemyCharacter.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

UWaitSpawnEnemies* UWaitSpawnEnemies::WaitSpawnEnemies(UGameplayAbility* OwningAbility, FGameplayTag EventTag,
                                                       TSoftClassPtr<AIPEnemyCharacter> SoftEnemyClassToSpawn, int32 NumToSpawn, const FVector& SpawnOrigin,
                                                       float RandomSpawnRadius)
{
	UWaitSpawnEnemies* Node = NewAbilityTask<UWaitSpawnEnemies>(OwningAbility);
	Node->CachedEventTag = EventTag;
	Node->CachedSoftEnemyClassToSpawn = SoftEnemyClassToSpawn;
	Node->CachedNumToSpawn = NumToSpawn;
	Node->CachedSpawnOrigin = SpawnOrigin;
	Node->CachedRandomSpawnRadius = RandomSpawnRadius;

	return Node;
}

void UWaitSpawnEnemies::Activate()
{
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	DelegateHandle = Delegate.AddUObject(this, &ThisClass::OnGameplayEventReceived);
}

void UWaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	Delegate.Remove(DelegateHandle);

	Super::OnDestroy(bInOwnerFinished);
}

void UWaitSpawnEnemies::OnGameplayEventReceived(const FGameplayEventData* InPayload)
{
	if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
	{
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &ThisClass::OnEnemyClassLoaded)
		);
	}
	
	else//CachedSoftEnemyClassToSpawn가 존재하지 않는다면
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AIPEnemyCharacter*>());
		}
		EndTask();//태스크 끝
	}
	
}

void UWaitSpawnEnemies::OnEnemyClassLoaded()
{
	UClass* LoadedClass = CachedSoftEnemyClassToSpawn.Get();
	UWorld* World = GetWorld();

	if (!LoadedClass || !World)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AIPEnemyCharacter*>());
		}

		EndTask();

		return;
	}

	TArray<AIPEnemyCharacter*> SpawnedEnemies;

	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < CachedNumToSpawn; i++)
	{
		FVector RandomLocation;
		UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, CachedSpawnOrigin, RandomLocation, CachedRandomSpawnRadius);//K2라는 것은 blueprintCallablefunction임을 알려준다. 여기에선 그냥 Native버전을 호출하는 역할, 랜덤위치 지정해주는 미리 만들어진 함수

		RandomLocation += FVector(0.f, 0.f, 150.f);

		const FRotator SpawnFacingRotation = AbilitySystemComponent->GetAvatarActor()->GetActorForwardVector().ToOrientationRotator();//이전에 받아 놓은 회전값을 받지 않고 생성되기 직전에 보스의 회전을 받아서 정확한 방향을 설정해 스폰하게 된다.

		AIPEnemyCharacter* SpawnedEnemy = World->SpawnActor<AIPEnemyCharacter>(LoadedClass, RandomLocation, SpawnFacingRotation, SpawnParam);

		if (SpawnedEnemy)
		{
			SpawnedEnemies.Add(SpawnedEnemy); //게임모드에서 처리해야 할 Enemy 추가시킬 때 사용 
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (!SpawnedEnemies.IsEmpty())
		{
			OnSpawnFinished.Broadcast(SpawnedEnemies);
		}
		else
		{
			DidNotSpawn.Broadcast(TArray<AIPEnemyCharacter*>());
		}
	}
	EndTask();
}
