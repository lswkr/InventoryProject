// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/IPEnemyCharacter.h"

#include "IPGameplayTags.h"
#include "NavigationSystem.h"
#include "AbilitySystem/IPAbilitySystemComponent.h"
#include "AbilitySystem/IPAbilitySystemLibrary.h"
#include "AbilitySystem/IPAttributeSet.h"
#include "AbilitySystem/Data/LootInfo.h"
#include "AI/IPAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/IPItemBase.h"
#include "UI/Widgets/IPUserWidget.h"

AIPEnemyCharacter::AIPEnemyCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UIPAbilitySystemComponent>("AbilitySystemComponent");

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	AttributeSet = CreateDefaultSubobject<UIPAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
	
	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent> (TEXT("EnemyCombatComponent"));
	BaseWalkSpeed = 250.f;
}

void AIPEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AIController = Cast<AIPAIController>(NewController);
	AIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AIController->RunBehaviorTree(BehaviorTree);
	AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AIController->GetBlackboardComponent()->SetValueAsBool(FName("RangeAttacker"), CharacterClass == ECharacterClass::Ranged);

	InitAbilityActorInfo();
	UIPAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);	

}

int32 AIPEnemyCharacter::GetPlayerLevel_Implementation()
{
	return Level;
}

void AIPEnemyCharacter::Die(const FVector& DeathImpulse)
{
	
	if (AIController)
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	}

	if (ULootInfo* LootInfo = UIPAbilitySystemLibrary::GetLootInfo(this))
	{
		TArray<FLootSpawnInfo> Loots = LootInfo->GetLootInfoBasedOnLevel(Level);
		
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		Params.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;
		
		for (int32 i = 0;i<Loots.Num();i++)
		{
			FVector RandomLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, this->GetActorLocation(), RandomLocation, 100.f);
			RandomLocation += FVector(0.f, 0.f, 200.f);
			
			const FLootSpawnInfo& Info = Loots[i];
			GetWorld()->SpawnActor<AIPItemBase>(Info.ItemClass, RandomLocation, FRotator::ZeroRotator, Params);
		}
	}
	HealthBar->DestroyComponent();
	SetLifeSpan(3);
	Super::Die(DeathImpulse);
}

UPawnCombatComponent* AIPEnemyCharacter::GetPawnCombatComponent_Implementation()
{
	return EnemyCombatComponent;
}

void AIPEnemyCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AIPEnemyCharacter::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

TArray<UAnimMontage*> AIPEnemyCharacter::GetAttackMontages_Implementation()
{
	return AttackMontages; 
}

UAnimMontage* AIPEnemyCharacter::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

UAnimMontage* AIPEnemyCharacter::GetDeathMontage_Implementation()
{
	return DeathMontage;
}

FProjectileInfo AIPEnemyCharacter::GetProjectileInfo_Implementation() const
{
	return ProjectileInfo;
}


void AIPEnemyCharacter::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	//UE_LOG(LogTemp,Warning,TEXT("HitReactTagChanged"));
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}

void AIPEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	
	
	if (UIPUserWidget* IPUserWidget = Cast<UIPUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		IPUserWidget->SetWidgetController(this);
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
			&AIPEnemyCharacter::HitReactTagChanged
		);

		OnHealthChanged.Broadcast(IPAS->GetHealth());
		OnMaxHealthChanged.Broadcast(IPAS->GetMaxHealth());
	}

	LeftCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AIPEnemyCharacter::OnBodyCollisionBoxBeginOverlap);
   
	RightCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AIPEnemyCharacter::OnBodyCollisionBoxBeginOverlap);
	
	BodyCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AIPEnemyCharacter::OnBodyCollisionBoxBeginOverlap);
}

void AIPEnemyCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UIPAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	AbilitySystemComponent->RegisterGameplayTagEvent(FIPGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AIPEnemyCharacter::StunTagChanged);

	InitializeDefaultAttributes();
	OnASCRegistered.Broadcast(AbilitySystemComponent);
}

void AIPEnemyCharacter::InitializeDefaultAttributes() const
{	
	UIPAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AIPEnemyCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);

	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bIsStunned);
	}
	
}

void AIPEnemyCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UCombatInterface>())
	{
		if (UIPAbilitySystemLibrary::IsHostile(OtherActor, this))
		{
			EnemyCombatComponent->OnHitTargetActor(OtherActor);
		}
	}
}
