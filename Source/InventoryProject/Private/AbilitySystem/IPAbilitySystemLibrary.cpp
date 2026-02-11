// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/IPAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "IPGameplayTags.h"
#include "AbilitySystem/Data/BossCharacterInfo.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Characters/IPHeroCharacter.h"
#include "Game/IPGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/IPPlayerState.h"
#include "UI/HUD/IPHUD.h"
#include "UI/WidgetControllers/IPWidgetController.h"
#include "IPAbilityTypes.h"
#include "Game/IPDungeonGameMode.h"
#include "Game/LoadScreenSaveGame.h"
#include "Kismet/KismetMathLibrary.h"

bool UIPAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject,
                                                         FWidgetControllerParams& OutWCParams, AIPHUD*& OutIPHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutIPHUD = Cast<AIPHUD>(PC->GetHUD());
		if (OutIPHUD)
		{
			AIPPlayerState* PS = PC->GetPlayerState<AIPPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			UIPInventorySystemComponent* ISC = PC->GetPawn<AIPHeroCharacter>()->GetInventorySystemComponent();
			UIPQuickSlotComponent* QSC = PC->GetPawn<AIPHeroCharacter>()->GetQuickSlotComponent();
			
			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			OutWCParams.InventorySystemComponent = ISC;
			OutWCParams.QuickSlotComponent = QSC;
			
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UIPAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AIPHUD* IPHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, IPHUD))
	{
		return IPHUD->GetOverlayWidgetController(WCParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UIPAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AIPHUD* IPHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, IPHUD))
	{
		return IPHUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

USkillMenuWidgetController* UIPAbilitySystemLibrary::GetSkillMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AIPHUD* IPHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, IPHUD))
	{
		return IPHUD->GetSkillMenuWidgetController(WCParams);
	}
	return nullptr;
}

UIPInventoryWidgetController* UIPAbilitySystemLibrary::GetInventoryWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AIPHUD* IPHUD = nullptr;

	if (MakeWidgetControllerParams(WorldContextObject, WCParams, IPHUD))
	{
		return IPHUD->GetInventoryWidgetController(WCParams);
	}
	return nullptr;
}

void UIPAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AActor* AvatarActor = ASC->GetAvatarActor();

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UIPAbilitySystemLibrary::InitializeDefaultAttributesForBoss(const UObject* WorldContextObject,
	EBossClass BossClass, UAbilitySystemComponent* ASC)
{
	AActor* AvatarActor = ASC->GetAvatarActor();

	UBossCharacterInfo* BossCharacterClassInfo = GetBossCharacterInfo(WorldContextObject);
	FBossClassDefaultInfo BossDefaultInfo = BossCharacterClassInfo->GetBossClassDefaultInfo(BossClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(BossDefaultInfo.PrimaryAttributes, 1, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(BossCharacterClassInfo->SecondaryAttributes, 1, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(BossCharacterClassInfo->VitalAttributes, 1, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UIPAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ULoadScreenSaveGame* SaveGame)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;

	const FIPGameplayTags& GameplayTags = FIPGameplayTags::Get();

	const AActor* SourceAvatarActor = ASC->GetAvatarActor();

	FGameplayEffectContextHandle EffectContexthandle = ASC->MakeEffectContext();
	EffectContexthandle.AddSourceObject(SourceAvatarActor);

	const FGameplayEffectSpecHandle PrimarySpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->PrimaryAttributes_SetByCaller, 1.f, EffectContexthandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimarySpecHandle, GameplayTags.Attributes_Primary_Strength, SaveGame->Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimarySpecHandle, GameplayTags.Attributes_Primary_Intelligence, SaveGame->Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimarySpecHandle, GameplayTags.Attributes_Primary_Will, SaveGame->Will);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimarySpecHandle, GameplayTags.Attributes_Primary_Luck, SaveGame->Luck);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimarySpecHandle.Data);

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(SourceAvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes_Infinite, 1.f, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	const FGameplayEffectSpecHandle VitalSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes_SetByCaller, 1.f, EffectContexthandle);
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(VitalSpecHandle, GameplayTags.Attributes_Vital_Health, SaveGame->HealthPoint);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(VitalSpecHandle, GameplayTags.Attributes_Vital_Mana, SaveGame->ManaPoint);
//	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes_SetByCaller, 1.f, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());
}
void UIPAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC,
	ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

	if (CharacterClassInfo == nullptr) return;

	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}

	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

void UIPAbilitySystemLibrary::GiveStartupAbilitiesForBoss(const UObject* WorldContextObject,
	UAbilitySystemComponent* ASC, EBossClass CharacterClass)
{
	UBossCharacterInfo* BossCharacterClassInfo = GetBossCharacterInfo(WorldContextObject);

	if (BossCharacterClassInfo == nullptr) return;

	const FBossClassDefaultInfo& DefaultInfo = BossCharacterClassInfo->GetBossClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}

	for (TSubclassOf<UGameplayAbility> AbilityClass : BossCharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
}

UItemInfo* UIPAbilitySystemLibrary::GetItemInfo(const UObject* WorldContextObject)
{
	const AIPGameModeBase* IPGameMode = Cast<AIPGameModeBase> (UGameplayStatics::GetGameMode(WorldContextObject));
	if (IPGameMode == nullptr) return nullptr;
	return IPGameMode->ItemInfo;
}

UCharacterClassInfo* UIPAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AIPGameModeBase* IPGameMode = Cast<AIPGameModeBase> (UGameplayStatics::GetGameMode(WorldContextObject));
	if (IPGameMode == nullptr) return nullptr;
	return IPGameMode->CharacterClassInfo;
}

UBossCharacterInfo* UIPAbilitySystemLibrary::GetBossCharacterInfo(const UObject* WorldContextObject)
{
//	const AIPGameModeBase* IPGameMode = Cast<AIPGameModeBase> (UGameplayStatics::GetGameMode(WorldContextObject));
	const AIPDungeonGameMode* IPGameMode = Cast<AIPDungeonGameMode> (UGameplayStatics::GetGameMode(WorldContextObject));
	if (IPGameMode == nullptr) return nullptr;
	return IPGameMode->BossCharacterInfo;
}


UAbilityInfo* UIPAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AIPGameModeBase* IPGameMode = Cast<AIPGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (IPGameMode == nullptr) return nullptr;
	return IPGameMode->AbilityInfo;
}

ULootInfo* UIPAbilitySystemLibrary::GetLootInfo(const UObject* WorldContextObject)
{
	const AIPDungeonGameMode* IPGameMode = Cast<AIPDungeonGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (IPGameMode == nullptr) return nullptr;
	return IPGameMode->LootInfo;
}

bool UIPAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{ 
	if (const FIPGameplayEffectContext* IPEffectContext = static_cast<const FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return IPEffectContext->IsBlockedHit();
	}
	return false;
}

bool UIPAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FIPGameplayEffectContext* IPEffectContext = static_cast<const FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return IPEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UIPAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FIPGameplayEffectContext* IPEffectContext = static_cast<const FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return IPEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UIPAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FIPGameplayEffectContext* IPEffectContext = static_cast<const FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return IPEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UIPAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FIPGameplayEffectContext* IPEffectContext = static_cast<const FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return IPEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UIPAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FIPGameplayEffectContext* IPEffectContext = static_cast<const FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (IPEffectContext->GetDamageType().IsValid())
		{
			return *IPEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UIPAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FIPGameplayEffectContext* IPEffectContext = static_cast<const FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return IPEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UIPAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FIPGameplayEffectContext* IPEffectContext = static_cast<const FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return IPEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

bool UIPAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FIPGameplayEffectContext* IPEffectContext = static_cast<const FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return IPEffectContext->IsCriticalHit();
	}
	return false;
}

void UIPAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FIPGameplayEffectContext* IPEffectContext = static_cast<FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		IPEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UIPAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FIPGameplayEffectContext* IPEffectContext = static_cast<FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		IPEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UIPAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInSuccessfulDebuff)
{
	if (FIPGameplayEffectContext* IPEffectContext = static_cast<FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		IPEffectContext->SetIsSuccessfulDebuff(bInSuccessfulDebuff);
	}
}

void UIPAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDamage)
{
	if (FIPGameplayEffectContext* IPEffectContext = static_cast<FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		IPEffectContext->SetDebuffDamage(InDamage);
	}
}

void UIPAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	if (FIPGameplayEffectContext* IPEffectContext = static_cast<FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		IPEffectContext->SetDebuffDuration(InDuration);
	}
}

void UIPAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InFrequency)
{
	if (FIPGameplayEffectContext* IPEffectContext = static_cast<FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		IPEffectContext->SetDebuffFrequency(InFrequency);
	}
}

void UIPAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& InDamageType)
{
	if (FIPGameplayEffectContext* IPEffectContext = static_cast<FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		IPEffectContext->SetDamageType(DamageType);
	}
}

void UIPAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InImpulse)
{
	if (FIPGameplayEffectContext* IPEffectContext = static_cast<FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		IPEffectContext->SetDeathImpulse(InImpulse);
	}
}

void UIPAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InForce)
{
	if (FIPGameplayEffectContext* IPEffectContext = static_cast<FIPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		IPEffectContext->SetKnockbackForce(InForce);
	}
}

bool UIPAbilitySystemLibrary::IsHostile(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothArePlayers || bBothAreEnemies;
	return !bFriends;
}

FGameplayEffectContextHandle UIPAbilitySystemLibrary::ApplyDamageEffect(
	const FDamageEffectParams& DamageEffectParams)
{
	if (DamageEffectParams.TargetAbilitySystemComponent == nullptr || DamageEffectParams.TargetAbilitySystemComponent == nullptr)
		return FGameplayEffectContextHandle();
	
	const FIPGameplayTags& GameplayTags = FIPGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	
	FGameplayEffectContextHandle EffectContexthandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContexthandle.AddSourceObject(SourceAvatarActor);
	SetDeathImpulse(EffectContexthandle, DamageEffectParams.DeathImpulse);
	SetKnockbackForce(EffectContexthandle, DamageEffectParams.KnockbackForce);

	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContexthandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);
	
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return EffectContexthandle;
}

int32 UIPAbilitySystemLibrary::GetXPRewardForLevel(const UObject* WorldContextObject,int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;
	const float XPReward = CharacterClassInfo->XPReward.GetValueAtLevel(CharacterLevel);
	//UE_LOG(LogTemp, Warning, TEXT("GetXPRewardForLevel: %f"), XPReward);
	return static_cast<int32>(XPReward);
}

int32 UIPAbilitySystemLibrary::GetXPRewardOfBoss(const UObject* WorldContextObject,EBossClass CharacterClass)
{
	UBossCharacterInfo* BossClassInfo = GetBossCharacterInfo(WorldContextObject);
	if (BossClassInfo == nullptr) return 0;

	const FBossClassDefaultInfo Info = BossClassInfo->GetBossClassDefaultInfo(CharacterClass);
	const float XPReward = Info.XPReward;

	return static_cast<int32>(XPReward);
}

FGameplayTag UIPAbilitySystemLibrary::ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference)
{
	FIPGameplayTags GameplayTags = FIPGameplayTags::Get();
	if (InAttacker != nullptr && InVictim != nullptr) //check처리하면 Projectile은 살아있어도 Attacker가 죽을 때 문제 발생
	{
		
		const FVector VictimForward = InVictim->GetActorForwardVector();
		const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

		const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
		OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

		const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);

	
		if (CrossResult.Z < 0.f)
		{
			OutAngleDifference *= -1.f;
		}

		if (OutAngleDifference >= -45 && OutAngleDifference <= 45) 
		{
			return GameplayTags.Player_HitReact_Front;
		}
		else if (OutAngleDifference < -45 && OutAngleDifference >= -135.f) 
		{
			return GameplayTags.Player_HitReact_Left;
		}
		else if (OutAngleDifference < -135.f || OutAngleDifference>135.f)
		{
			return GameplayTags.Player_HitReact_Back;
		}
		else if (OutAngleDifference > 45.f && OutAngleDifference <= 135.f)
		{
			return GameplayTags.Player_HitReact_Right;
		}
		return GameplayTags.Player_HitReact_Front;
	}
	return GameplayTags.Player_HitReact_Front;
}

TArray<FVector> UIPAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors, float MinDistance, float MaxDistance)
{
	TArray<FVector> Vectors;
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumVectors > 1)
	{
		const float DeltaSpread = Spread / (NumVectors - 1);
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			int32 Dist = FMath::RandRange(FMath::Max(0, MinDistance),FMath::Max(MinDistance, MaxDistance));
			Vectors.Add(Direction*Dist);
		}
	}
	else
	{
		int32 Dist = FMath::RandRange(FMath::Max(0, MinDistance),FMath::Max(MinDistance, MaxDistance));
		Vectors.Add(Forward*Dist);
	}
	return Vectors;
}