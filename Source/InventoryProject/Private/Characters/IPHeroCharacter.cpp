// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/IPHeroCharacter.h"

#include "AbilitySystemComponent.h"
#include "IPGameplayTags.h"
#include "AbilitySystem/IPAbilitySystemComponent.h"
#include "AbilitySystem/IPAbilitySystemLibrary.h"
#include "AbilitySystem/IPAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/IPInventorySystemComponent.h"
#include "Components/IPQuickSlotComponent.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Game/IPGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/IPPlayerController.h"
#include "Player/IPPlayerState.h"
#include "UI/HUD/IPHUD.h"
#include "Particles/ParticleSystemComponent.h"

AIPHeroCharacter::AIPHeroCharacter()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());
	//SpringArm->SetUsingAbsoluteRotation(true);
	//SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	IPInventorySystemComponent = CreateDefaultSubobject<UIPInventorySystemComponent>(TEXT("IPInventorySystemComponent"));
	IPQuickSlotComponent = CreateDefaultSubobject<UIPQuickSlotComponent>(TEXT("IPQuickSlotComponent"));
	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent> (TEXT("HeroCombatComponent"));

	LevelUpParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LevelUpParticleComponent"));
	LevelUpParticleComponent->SetupAttachment(GetRootComponent());
	LevelUpParticleComponent->bAutoActivate = false;
	
	BaseWalkSpeed = 700.f;
	BaseRotationRate = FRotator(0.f, 500.f, 0.f);
}

void AIPHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AIPPlayerState* IPPlayerState = GetPlayerState<AIPPlayerState>())
	{
		// InitAbilityActorInfo();
		// InitializeDefaultAttributes();
		// AddCharacterAbilities();
		InitAbilityActorInfo();
		LoadProgress();
		if (AIPGameModeBase* AuraGameMode = Cast<AIPGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGameMode->LoadWorldState(GetWorld());
		}
	}
}


void AIPHeroCharacter::AddToXP_Implementation(int32 InXP)
{
	AIPPlayerState* IPPlayerState = GetPlayerState<AIPPlayerState>();
	check(IPPlayerState);
	IPPlayerState->AddToXP(InXP);
}

void AIPHeroCharacter::LevelUp_Implementation()
{
	if (IsValid(LevelUpParticleComponent))
	{
		LevelUpParticleComponent->SetWorldLocation(this->GetActorLocation());
		LevelUpParticleComponent->Activate(true);
		//LevelUp시 DefaultAbility들은 레벨업. PassiveAbilities들은 레벨업 필요없는 것들이라 레벨업 X
		for (TSubclassOf<UGameplayAbility> Ability: StartupAbilities)
		{
			FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(Ability);
			if(AbilitySpec)
			{
				AbilitySpec->Level++;
				AbilitySystemComponent->MarkAbilitySpecDirty(*AbilitySpec);
			}
		}
	}
}

int32 AIPHeroCharacter::GetXP_Implementation() const
{
	AIPPlayerState* IPPlayerState = GetPlayerState<AIPPlayerState>();
	check(IPPlayerState);
	return IPPlayerState->GetXP();
}

int32 AIPHeroCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	AIPPlayerState* IPPlayerState = GetPlayerState<AIPPlayerState>();
	check(IPPlayerState);
	return IPPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AIPHeroCharacter::GetAttributePointsReward_Implementation(int32 InLevel) const
{
	AIPPlayerState* IPPlayerState = GetPlayerState<AIPPlayerState>();
	check(IPPlayerState);
	return IPPlayerState->LevelUpInfo->LevelUpInformation[InLevel].AttributePointReward;
}

int32 AIPHeroCharacter::GetSkillPointsReward_Implementation(int32 InLevel) const
{
	AIPPlayerState* IPPlayerState = GetPlayerState<AIPPlayerState>();
	check(IPPlayerState);
	return IPPlayerState->LevelUpInfo->LevelUpInformation[InLevel].SkillPointReward;
}

void AIPHeroCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AIPPlayerState* IPPlayerState = GetPlayerState<AIPPlayerState>();
	check(IPPlayerState);
	IPPlayerState->AddToLevel(InPlayerLevel);

	if (UIPAbilitySystemComponent* IPASC = Cast<UIPAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		IPASC->UpdateAbilityStatuses(IPPlayerState->GetPlayerLevel());
	}
}

void AIPHeroCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AIPPlayerState* IPPlayerState = GetPlayerState<AIPPlayerState>();
	check(IPPlayerState);
	IPPlayerState->AddToAttributePoints(InAttributePoints);
}

void AIPHeroCharacter::AddToSkillPoints_Implementation(int32 InSkillPoints)
{
	AIPPlayerState* IPPlayerState = GetPlayerState<AIPPlayerState>();
	check(IPPlayerState);
	IPPlayerState->AddToSkillPoints(InSkillPoints);
}

int32 AIPHeroCharacter::GetAttributePoints_Implementation() const
{
	AIPPlayerState* IPPlayerState = GetPlayerState<AIPPlayerState>();
	check(IPPlayerState);
	return IPPlayerState->GetAttributePoints();
}

int32 AIPHeroCharacter::GetSkillPoints_Implementation() const
{
	AIPPlayerState* IPPlayerState = GetPlayerState<AIPPlayerState>();
	check(IPPlayerState);
	return IPPlayerState->GetSkillPoints();
}

void AIPHeroCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	AIPGameModeBase* IPGameMode = Cast<AIPGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (IPGameMode)
	{
		ULoadScreenSaveGame* SaveData = IPGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;

		SaveData->PlayerStartTag = CheckpointTag;

		if (AIPPlayerState* IPPlayerState = Cast<AIPPlayerState>(GetPlayerState()))
		{
			SaveData->PlayerLevel = IPPlayerState->GetPlayerLevel();
			SaveData->XP = IPPlayerState->GetXP();
			SaveData->AttributePoints = IPPlayerState->GetAttributePoints();
			SaveData->SkillPoints = IPPlayerState->GetSkillPoints();
		}
		SaveData->Strength = UIPAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Intelligence = UIPAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Will = UIPAttributeSet::GetWillAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Luck = UIPAttributeSet::GetLuckAttribute().GetNumericValue(GetAttributeSet());
		SaveData->HealthPoint = UIPAttributeSet::GetHealthAttribute().GetNumericValue(GetAttributeSet());
		SaveData->ManaPoint = UIPAttributeSet::GetManaAttribute().GetNumericValue(GetAttributeSet());
		
		SaveData->bFirstTimeLoadIn = false;
		
		UIPAbilitySystemComponent* IPASC = Cast<UIPAbilitySystemComponent>(AbilitySystemComponent);
		FForEachAbility SaveAbilityDelegate;
		SaveData->SavedAbilities.Empty();
		SaveAbilityDelegate.BindLambda([this, IPASC, SaveData](const FGameplayAbilitySpec& AbilitySpec)
		{
			const FGameplayTag AbilityTag = IPASC->GetAbilityTagFromSpec(AbilitySpec);
			UAbilityInfo* AbilityInfo = UIPAbilitySystemLibrary::GetAbilityInfo(this);
			FIPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);

			FSavedAbility SavedAbility;
			SavedAbility.GameplayAbility = Info.Ability;
			SavedAbility.AbilityLevel = AbilitySpec.Level;
			SavedAbility.AbilitySlot = IPASC->GetSlotFromAbilityTag(AbilityTag);
			SavedAbility.AbilityStatus = IPASC->GetStatusFromAbilityTag(AbilityTag);
			SavedAbility.AbilityTag = AbilityTag;
			SavedAbility.AbilityType = Info.AbilityType;
			SaveData->SavedAbilities.AddUnique(SavedAbility);

		});
		IPASC->ForEachAbility(SaveAbilityDelegate);

		UIPInventorySystemComponent* IPISC = Cast<UIPInventorySystemComponent>(IPInventorySystemComponent);
		FForEachSlot SaveSlotDelegate;
		SaveData->SavedItems.Empty();
		SaveSlotDelegate.BindLambda([this, IPISC, SaveData](const FInventorySlot& Slot, int32 Index)
		{
			FSavedItem SavedItem;
			SavedItem.ItemName = Slot.ItemName;
			SavedItem.Index = Index;
			SavedItem.QuickSlotIndex = Slot.QuickSlotIndex;
			SavedItem.Amount = Slot.Amount;
			SaveData->SavedItems.AddUnique(SavedItem);
		});

		IPISC->ForEachSlot(SaveSlotDelegate);

		UIPQuickSlotComponent* IPQSC = Cast<UIPQuickSlotComponent>(IPQuickSlotComponent);
		
		FForEachQuickSlot SaveQuickSlotDelegate;
		SaveData->SavedQuickSlots.Empty();
		SaveQuickSlotDelegate.BindLambda([this, IPQSC, SaveData](const FQuickSlot& Slot,const int32& QuickSlotIndex)
		{
			FSavedQuickSlot SavedQuickSlot;
			SavedQuickSlot.ItemName = Slot.ItemName;
			SavedQuickSlot.InventoryIndex = Slot.InventoryIndex;
			SavedQuickSlot.QuickSlotIndex = QuickSlotIndex;
			SavedQuickSlot.Amount = Slot.Amount;
			SaveData->SavedQuickSlots.AddUnique(SavedQuickSlot);
			//UE_LOG(LogTemp, Warning, TEXT("saved QuickSlot Name: %s, Amount: %d"), *Slot.ItemName.ToString(), Slot.Amount);
		});
		IPQSC->ForEachQuickSlot(SaveQuickSlotDelegate);

		IPGameMode->SaveInGameProgressData(SaveData);
	}
}

UAnimMontage* AIPHeroCharacter::GetHitReactMontage_Implementation(int32 Index)
{
	checkf(HitReactMontages.Num()>0, TEXT("HitReactMontages has no elements."));
	
	if (Index<0 || Index>=HitReactMontages.Num()) return HitReactMontages[0];
	return HitReactMontages[Index];
}

int32 AIPHeroCharacter::GetPlayerLevel_Implementation()
{
	const AIPPlayerState* IPPlayerState = GetPlayerState<AIPPlayerState>();
	check(IPPlayerState);
	return IPPlayerState->GetPlayerLevel();
}

void AIPHeroCharacter::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);

	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda([this]()
	{
		AIPGameModeBase* IPGM = Cast<AIPGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (IPGM)
		{
			IPGM->PlayerDied(this);
		}
	});
	if (AbilitySystemComponent)
	{
		if (!AbilitySystemComponent->HasMatchingGameplayTag(FIPGameplayTags::Get().Status_Dead))//죽으면 Health, Mana Regeneration안 되도록 태그 추가
		{
			AbilitySystemComponent->AddLooseGameplayTag(FIPGameplayTags::Get().Status_Dead);
		}
	}
	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, DeathTime, false);
	Camera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	APlayerController* PC = Cast<APlayerController>(GetController());
	PC->DisableInput(PC);//죽은 뒤 움직이지 못하도록
}

UPawnCombatComponent* AIPHeroCharacter::GetPawnCombatComponent_Implementation()
{
	return HeroCombatComponent;
}

void AIPHeroCharacter::LoadProgress()
{
	AIPGameModeBase* IPGameMode = Cast<AIPGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (IPGameMode)
	{
		ULoadScreenSaveGame* SaveData = IPGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;

		if (SaveData->bFirstTimeLoadIn)
		{
			InitializeDefaultAttributes();
			AddCharacterAbilities();
			ApplyEffectToSelf(RegenerationEffect, 1.f);
			
			if (AIPPlayerState* IPPlayerState = Cast<AIPPlayerState>(GetPlayerState()))
			{
				IPPlayerState->SetCharacterName(SaveData->PlayerName);
				IPPlayerState->SetLevel(Level);
			}
		}
		else
		{
			if (UIPAbilitySystemComponent* IPASC = Cast<UIPAbilitySystemComponent>(AbilitySystemComponent))
			{
				IPASC->AddCharacterAbilitiesFromSaveData(SaveData);
			}

			if (UIPInventorySystemComponent* IPISC = Cast<UIPInventorySystemComponent>(IPInventorySystemComponent))
			{
				IPInventorySystemComponent->SetInventorySize(InventorySize);
				IPISC->AddCharacterItemsFromSaveData(SaveData);
			}

			if (UIPQuickSlotComponent* IPQSC = Cast<UIPQuickSlotComponent>(IPQuickSlotComponent))
			{
				//IPQuickSlotComponent->SetQuickslot(InventorySize);
				IPQSC->AddCharacterQuickSlotsFromSaveData(SaveData);
			}
			
			UIPAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(this, AbilitySystemComponent, SaveData); // Regeneration때문에 Attribute들 미리 초기화 시켜놓아야 함
			
			if (AIPPlayerState* IPPlayerState = Cast<AIPPlayerState>(GetPlayerState()))
			{
				IPPlayerState->SetLevel(SaveData->PlayerLevel);
				IPPlayerState->SetXP(SaveData->XP);
				IPPlayerState->SetAttributePoints(SaveData->AttributePoints);
				IPPlayerState->SetSkillPoints(SaveData->SkillPoints);
				IPPlayerState->SetCharacterName(SaveData->PlayerName);
				ApplyEffectToSelf(RegenerationEffect, SaveData->PlayerLevel);
			}
			
		}
	}
}

void AIPHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	GetCharacterMovement()->RotationRate = BaseRotationRate;
	if (IPInventorySystemComponent->GetInventorySize()==0)
	{
		IPInventorySystemComponent->SetInventorySize(InventorySize);
	}

	LeftCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AIPHeroCharacter::OnBodyCollisionBoxBeginOverlap);
    LeftCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &AIPHeroCharacter::OnBodyCollisionBoxEndOverlap);
   
    RightCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AIPHeroCharacter::OnBodyCollisionBoxBeginOverlap);
    RightCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &AIPHeroCharacter::OnBodyCollisionBoxEndOverlap);
   
    BodyCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AIPHeroCharacter::OnBodyCollisionBoxBeginOverlap);
    BodyCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &AIPHeroCharacter::OnBodyCollisionBoxEndOverlap);
}

void AIPHeroCharacter::InitAbilityActorInfo()
{
	AIPPlayerState* IPPlayerState = GetPlayerState<AIPPlayerState>();
	check(IPPlayerState);
	IPPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(IPPlayerState, this);
	Cast<UIPAbilitySystemComponent>(IPPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet(); //effect의 asset태그 중 message있으면 델리게이트 하도록 하는 것
	AbilitySystemComponent = IPPlayerState->GetAbilitySystemComponent();
	AttributeSet = IPPlayerState->GetAttributeSet();
	OnASCRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FIPGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AIPHeroCharacter::StunTagChanged);
	
	if (AIPPlayerController* IPPlayerController = Cast<AIPPlayerController>(GetController()))
	{
		if (AIPHUD* IPHUD = Cast<AIPHUD>(IPPlayerController->GetHUD()))
		{
			IPHUD->InitOverlay(IPPlayerController, IPPlayerState, AbilitySystemComponent, AttributeSet, IPInventorySystemComponent, IPQuickSlotComponent);
		}
	}
}

void AIPHeroCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UCombatInterface>())
	{
		if (UIPAbilitySystemLibrary::IsHostile(OtherActor, this))
		{
			HeroCombatComponent->OnHitTargetActor(OtherActor);
		}
	}
	
}

void AIPHeroCharacter::OnBodyCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->Implements<UCombatInterface>())
	{
		if (UIPAbilitySystemLibrary::IsHostile(OtherActor, this))
		{
			HeroCombatComponent->OnHitBoxPulledFromTargetActor(OtherActor);
		}
	}
}

void AIPHeroCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);
	GetCharacterMovement()->RotationRate = bIsStunned ? FRotator::ZeroRotator : BaseRotationRate;
}

