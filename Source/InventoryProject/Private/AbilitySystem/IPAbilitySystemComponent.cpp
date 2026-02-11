// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/IPAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "IPGameplayTags.h"
#include "AbilitySystem/IPAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/IPGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Game/LoadScreenSaveGame.h"
#include "Interfaces/PlayerInterface.h"

void UIPAbilitySystemComponent::AbilityActorInfoSet()
{
	FIPGameplayTags GameplayTags = FIPGameplayTags::Get();

	for (const FIPAbilityInfo& Info : UIPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor())->AbilityInformation)
	{
		CurrentAbilitiesState.Add(Info.AbilityTag,0);
	}
	
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UIPAbilitySystemComponent::EffectApplied);
}

void UIPAbilitySystemComponent::AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveData)
{
//	UE_LOG(LogTemp, Warning, TEXT("AddCharacterAbilitiesFromSaveData Activated"));
	for (const FSavedAbility& Data : SaveData->SavedAbilities)
	{
		
		const TSubclassOf<UGameplayAbility> LoadedAbilityClass = Data.GameplayAbility;
		
		FGameplayAbilitySpec LoadedAbilitySpec = FGameplayAbilitySpec(LoadedAbilityClass, Data.AbilityLevel);

		LoadedAbilitySpec.DynamicAbilityTags.AddTag(Data.AbilitySlot);
		LoadedAbilitySpec.DynamicAbilityTags.AddTag(Data.AbilityStatus);

		if (Data.AbilityType == FIPGameplayTags::Get().Abilities_Type_Active) //다른 어빌리티는 넣기만
		{
			//UE_LOG(LogTemp, Warning, TEXT("saved Ability: %s"), *Data.AbilityTag.ToString());
			GiveAbility(LoadedAbilitySpec);
		}
		else //ListenForEvent같은 PassiveAbility는 바로 Activate 
		{
			GiveAbilityAndActivateOnce(LoadedAbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UIPAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	//UE_LOG(LogTemp, Warning, TEXT("AddCharacterAbilities Activated"));
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UIPGameplayAbility* IPAbility = Cast<UIPGameplayAbility>(AbilitySpec.Ability))
		{
		//	UE_LOG(LogTemp, Warning, TEXT("%s"),*IPAbility->GetName());
			AbilitySpec.DynamicAbilityTags.AddTag(IPAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FIPGameplayTags::Get().Abilities_Status_Equipped);
			
			GiveAbility(AbilitySpec);
		}
	}

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast(); 
	}
	
}

void UIPAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		AbilitySpec.DynamicAbilityTags.AddTag(FIPGameplayTags::Get().Abilities_Status_Equipped);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UIPAbilitySystemComponent::AddCharacterAbilities_forBoss(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UIPGameplayAbility* IPAbility = Cast<UIPGameplayAbility>(AbilitySpec.Ability))
		{
			//UE_LOG(LogTemp, Warning, TEXT("%s"),*IPAbility->GetName());;
			GiveAbility(AbilitySpec);
		}
	}
}

void UIPAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
	
}

void UIPAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UIPAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void UIPAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
	UAbilityInfo* AbilityInfo = UIPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	FIPGameplayTags GameplayTags = FIPGameplayTags::Get();
	
	for (const FIPAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{

		FGameplayTag Status = GetStatusFromAbilityTag(Info.AbilityTag);
		if (Status != GameplayTags.Abilities_Status_Equipped && Status != GameplayTags.Abilities_Status_Unlocked && Status != GameplayTags.Abilities_Status_Eligible)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Locked Ability: %s"), *Info.AbilityTag.ToString());
			AbilityStatusChanged.Broadcast(Info.AbilityTag, Status, 0);
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Equipped Ability: %s"), *Info.AbilityTag.ToString());
			AbilityStatusChanged.Broadcast(Info.AbilityTag, Status, GetSpecFromAbilityTag(Info.AbilityTag)->Level);
		}
		
	}
}

FGameplayTag UIPAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UIPAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UIPAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.DynamicAbilityTags)
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UIPAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusFromSpec(*Spec);
	}
	return FGameplayTag();
}

FGameplayTag UIPAbilitySystemComponent::GetSlotFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

bool UIPAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (DoesAbilityHaveSlot(AbilitySpec, Slot))
		{
			return false;
		}
	}
	return true;
}

bool UIPAbilitySystemComponent::DoesAbilityHaveSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	return Spec.DynamicAbilityTags.HasTagExact(Slot);
}

bool UIPAbilitySystemComponent::DoesAbilityHaveAnySlot(const FGameplayAbilitySpec& Spec)
{
	return Spec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

FGameplayAbilitySpec* UIPAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(Slot))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

void UIPAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	ClearSlot(&Spec);
	Spec.DynamicAbilityTags.AddTag(Slot);
}

FGameplayAbilitySpec* UIPAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

void UIPAbilitySystemComponent::UpdateAbilityStatuses(int32 Level) //레벨업 했을 때 전체 스킬의 레벨 제한이 풀렸는지 보기 위한 업데이트
{
	UAbilityInfo* AbilityInfo = UIPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (FIPAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (!Info.AbilityTag.IsValid()) continue;
		if (Level < Info.LevelRequirement) continue;
		
		if (bIsAbilityQualified(Info.AbilityTag))
		{
			//UE_LOG(LogTemp, Warning, TEXT("%s is qualified"),*Info.AbilityTag.ToString());
			FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(Info.AbilityTag);
			if (Spec == nullptr)
			{
				FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
				AbilitySpec.DynamicAbilityTags.AddTag(FIPGameplayTags::Get().Abilities_Status_Eligible);
				Info.StatusTag = FIPGameplayTags::Get().Abilities_Status_Eligible;
				GiveAbility(AbilitySpec);
				MarkAbilitySpecDirty(AbilitySpec);
				
				AbilityStatusChanged.Broadcast(Info.AbilityTag,FIPGameplayTags::Get().Abilities_Status_Eligible, 1);
			}
			else
			{
				FGameplayTag CurrentStatus = GetStatusFromAbilityTag(Info.AbilityTag);
				Info.StatusTag = CurrentStatus;
				AbilityStatusChanged.Broadcast(Info.AbilityTag,CurrentStatus, Spec->Level);
			}
		}
	}
}

// bool UIPAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription)
// {
// 	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
// 	{
// 		if(UIPGameplayAbility* AuraAbility = Cast<UIPGameplayAbility>(AbilitySpec->Ability))
// 		{
// 			OutDescription = AuraAbility->GetDescription(AbilitySpec->Level);
// 			return true;
// 		}
// 	}
// 	
// 	const UAbilityInfo* AbilityInfo = UIPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
// 	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FIPGameplayTags::Get().Abilities_None))
// 	{
// 		OutDescription = FString();
// 	}
// 	else
// 	{
// 		OutDescription = UIPGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
// 	}
// 	
//
// 	return false;
// }

void UIPAbilitySystemComponent::SpendSkillPoint(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSkillPoints(GetAvatarActor(), -1);
		}

		const FIPGameplayTags GameplayTags = FIPGameplayTags::Get();
		FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);
		if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible)) //레벨 1로 시작
		{
			//UE_LOG(LogTemp,Warning, TEXT("%s is Eligible"), *AbilityTag.ToString());
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
			Status = GameplayTags.Abilities_Status_Unlocked;
		}
		else if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped)||Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{//어빌리티 레벨 1 이후에는 레벨 올리기
			//UE_LOG(LogTemp,Warning, TEXT("%s is Equipped or Unlocked"), *AbilityTag.ToString());
			AbilitySpec->Level ++;
			FIPAbilityInfo AbilityInfo = UIPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor())->FindAbilityInfoForTag(GetAbilityTagFromSpec(*AbilitySpec));
			AbilityInfo.AbilityLevel = AbilitySpec->Level;
		
		}
		CurrentAbilitiesState[AbilityTag]++;
		AbilityStatusChanged.Broadcast(AbilityTag, Status, AbilitySpec->Level);
		MarkAbilitySpecDirty(*AbilitySpec);
		
		//선행 스킬 레벨 변경 후 조건맞는 Post스킬 ASC에 넣기
		for (const FIPAbilityInfo& AbilityInfo : UIPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor())->AbilityInformation)
		{
			if (AbilityInfo.AbilityTag == AbilityTag)
			{
				for (FAbilityStackInfo StackInfo : AbilityInfo.PostAbilityTags)
				{
					if (bIsAbilityQualified(StackInfo.AbilityTag))
					{	
						if (GetSpecFromAbilityTag(StackInfo.AbilityTag)==nullptr)
						{
							FGameplayAbilitySpec PostSpec = FGameplayAbilitySpec(StackInfo.Ability, 1);//초기 Level1
							PostSpec.DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Eligible);
							
							GiveAbility(PostSpec);
							MarkAbilitySpecDirty(PostSpec);
							AbilityStatusChanged.Broadcast(StackInfo.AbilityTag, GameplayTags.Abilities_Status_Eligible, 0);
						}
					}
				}
			}
		}
		
		
	}
	
}

void UIPAbilitySystemComponent::EquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Slot)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FIPGameplayTags& GameplayTags = FIPGameplayTags::Get();

		//Equip하려는 Ability의 현 상태
		const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag& Status = GetStatusFromSpec(*AbilitySpec);

		const bool bStatusValid = Status == GameplayTags.Abilities_Status_Equipped || Status == GameplayTags.Abilities_Status_Unlocked; 

		if (bStatusValid)//등록이 가능한 어빌리티인지 확인. 있다면 지우기.
		{
			if (!SlotIsEmpty(Slot)) //슬롯(InputTag)에 Ability가 있다면
			{
				FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(Slot);
				if (SpecWithSlot)
				{
					//해당 슬롯에 있는 어빌리티가 등록하려는 어빌리티와 같다면 빠른 return;
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
					{
						AbilityEquipped.Broadcast(AbilityTag,  GameplayTags.Abilities_Status_Equipped, Slot, PrevSlot);
						return;
					}
					ClearSlot(SpecWithSlot);
				}
			}

			if (!DoesAbilityHaveAnySlot(*AbilitySpec)) //해당 어빌리티가 등록되어 있지 않다면
			{
				AbilitySpec->DynamicAbilityTags.RemoveTag(GetStatusFromSpec(*AbilitySpec));
				AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Equipped);
			}
			AssignSlotToAbility(*AbilitySpec, Slot);
			MarkAbilitySpecDirty(*AbilitySpec);
		}
		AbilityEquipped.Broadcast(AbilityTag,  GameplayTags.Abilities_Status_Equipped, Slot, PrevSlot);
	}
}

void UIPAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag Slot = GetInputTagFromSpec(*Spec);
	Spec->DynamicAbilityTags.RemoveTag(Slot);
}

void UIPAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (DoesAbilityHaveSlot(&Spec, Slot))
		{
			ClearSlot(&Spec);
		}
	}
}

bool UIPAbilitySystemComponent::DoesAbilityHaveSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	for (FGameplayTag Tag : Spec->DynamicAbilityTags)
	{
		if (Tag.MatchesTagExact(Slot))
		{
			return true;
		}
	}
	return false;
}

bool UIPAbilitySystemComponent::bIsAbilityQualified(const FGameplayTag& AbilityTag)
{
	FIPGameplayTags GameplayTags = FIPGameplayTags::Get();
	for (const FIPAbilityInfo& Info : UIPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor())->AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			FGameplayTag Status = GetStatusFromAbilityTag(AbilityTag);
			if (Status ==  GameplayTags.Abilities_Status_Equipped ||Status ==  GameplayTags.Abilities_Status_Unlocked) return true;
			
			for (const FAbilityStackInfo& StackInfo : Info.PreAbilityTags) //선행 스킬의 조건을 모두 충족시키는지 확인
			{
				if (CurrentAbilitiesState[StackInfo.AbilityTag] < StackInfo.StackSize) //충족 못 시킨 경우 false
				{
				//	UE_LOG(LogTemp, Warning, TEXT("%s is not qualified"),*Info.AbilityTag.ToString());
					return false;
				}
				
			}
			return true;//모두 충족 시켰으면 true
		}
	}
	return false;
	
}

void UIPAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0) //포인트가 0보다 많다면
		{
			FGameplayEventData Payload;
			Payload.EventTag = AttributeTag;
			Payload.EventMagnitude = 1.f;	//해당 Attribute에 +1

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

			if (GetAvatarActor()->Implements<UPlayerInterface>())
			{
				IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1); //총 포인트에서 -1
			}
		}
	}
}

void UIPAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                              const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}
