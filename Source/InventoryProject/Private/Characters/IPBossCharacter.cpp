// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/IPBossCharacter.h"

#include "IPGameplayTags.h"
#include "AbilitySystem/IPAbilitySystemComponent.h"
#include "AbilitySystem/IPAbilitySystemLibrary.h"
#include "AbilitySystem/IPAttributeSet.h"
#include "AI/IPAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/IPEnemyCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widgets/IPUserWidget.h"

AIPBossCharacter::AIPBossCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UIPAbilitySystemComponent>("AbilitySystemComponent");

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	AttributeSet = CreateDefaultSubobject<UIPAttributeSet>("AttributeSet");

	BossCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent> (TEXT("BossCombatComponent"));
	BaseWalkSpeed = 250.f;
}


EBossClass AIPBossCharacter::GetBossClass_Implementation()
{
	return BossClass;
}

void AIPBossCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AIController = Cast<AIPAIController>(NewController);
	AIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AIController->RunBehaviorTree(BehaviorTree);

	InitAbilityActorInfo();
	UIPAbilitySystemLibrary::GiveStartupAbilitiesForBoss(this, AbilitySystemComponent, BossClass);	
}

void AIPBossCharacter::Die(const FVector& DeathImpulse)
{
	if (AIController)
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	}

	if (BossHealthBarWidget)
	{
		BossHealthBarWidget->RemoveFromParent();
	}
	SetLifeSpan(3);
	Super::Die(DeathImpulse);
}

UPawnCombatComponent* AIPBossCharacter::GetPawnCombatComponent_Implementation()
{
	return BossCombatComponent;
}

void AIPBossCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AIPBossCharacter::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}


UAnimMontage* AIPBossCharacter::GetDeathMontage_Implementation()
{
	return DeathMontage;
}

FProjectileInfo AIPBossCharacter::GetProjectileInfo_Implementation() const
{
	return ProjectileInfo;
}

TArray<UAnimMontage*> AIPBossCharacter::GetMeleeAttackMontages_Implementation()
{
	return MeleeAttackMontages;
}

TArray<UAnimMontage*> AIPBossCharacter::GetRangeAttackMontages_Implementation()
{
	return RangeAttackMontages;
}

UAnimMontage* AIPBossCharacter::GetSummonMontage_Implementation()
{
	return SummonEnemiesMontage;
}

FSummonEnemyInfo AIPBossCharacter::GetSummonEnemyInfo_Implementation()
{
	return SummonEnemyInfo;
}


void AIPBossCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	
	if (!BossHealthBarWidget)
	{
		checkf(BossHealthBarWidgetClass, TEXT("BossHealthBarWidget didn't set"));
		
		BossHealthBarWidget = CreateWidget<UIPUserWidget>(GetWorld(), BossHealthBarWidgetClass);
		BossHealthBarWidget->SetWidgetController(this);
		BossHealthBarWidget->AddToViewport();
	}
	
	if (const UIPAttributeSet* IPAS = Cast<UIPAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(IPAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(IPAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		
		AbilitySystemComponent->RegisterGameplayTagEvent(FIPGameplayTags::Get().Effect_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AIPBossCharacter::HitReactTagChanged
		);

		OnHealthChanged.Broadcast(IPAS->GetHealth());
		OnMaxHealthChanged.Broadcast(IPAS->GetMaxHealth());
	}
	
	BossNameDelegate.Broadcast(BossName);


	LeftCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AIPBossCharacter::OnBodyCollisionBoxBeginOverlap);
	RightCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AIPBossCharacter::OnBodyCollisionBoxBeginOverlap);
	BodyCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AIPBossCharacter::OnBodyCollisionBoxBeginOverlap);
	
}

void AIPBossCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UIPAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	AbilitySystemComponent->RegisterGameplayTagEvent(FIPGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AIPBossCharacter::StunTagChanged);

	InitializeDefaultAttributesForBoss();
	OnASCRegistered.Broadcast(AbilitySystemComponent);
}

void AIPBossCharacter::InitializeDefaultAttributesForBoss() const
{
	UIPAbilitySystemLibrary::InitializeDefaultAttributesForBoss(this, BossClass, AbilitySystemComponent);
}

void AIPBossCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UCombatInterface>())
	{
		if (UIPAbilitySystemLibrary::IsHostile(OtherActor, this))
		{
			BossCombatComponent->OnHitTargetActor(OtherActor);
		}
	}
}


void AIPBossCharacter::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}

void AIPBossCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);

	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bIsStunned);
	}
	
}