// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/IPCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "IPGameplayTags.h"
#include "AbilitySystem/IPAbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MotionWarpingComponent.h" 
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

// Sets default values
AIPCharacterBase::AIPCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	LeftCollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("LeftCollisionBox"));
	LeftCollisionBox->SetupAttachment(GetMesh());
	LeftCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RightCollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("RightCollisionBox"));
	RightCollisionBox->SetupAttachment(GetMesh());
	RightCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BodyCollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("BodyCollisionBox"));
	BodyCollisionBox->SetupAttachment(GetMesh());
	BodyCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("StunDebuffComponent");
	StunDebuffComponent->SetupAttachment(GetRootComponent());
	StunDebuffComponent->DebuffTag = FIPGameplayTags::Get().Debuff_Stun;
	
	BleedingDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BleedingDebuffComponent");
	BleedingDebuffComponent->SetupAttachment(GetRootComponent());
	BleedingDebuffComponent->DebuffTag = FIPGameplayTags::Get().Debuff_Bleeding;
	
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}



UAbilitySystemComponent* AIPCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AIPCharacterBase::Die(const FVector& DeathImpulse)
{
	HandleDeath(DeathImpulse);
}

FOnDeathSignature& AIPCharacterBase::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

bool AIPCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AIPCharacterBase::GetAvatar_Implementation()
{
	return this;
}

FOnASCRegistered& AIPCharacterBase::GetOnASCRegisteredDelegate()
{
	return OnASCRegistered;
}

FOnDamageSignature& AIPCharacterBase::GetOnDamageSignature()
{
	return OnDamageDelegate;
}

FVector AIPCharacterBase::GetCombatSocketLocation_Implementation()
{
	if (ProjectileSocketName!=TEXT("None"))
	{
		return  GetMesh()->GetSocketLocation(ProjectileSocketName);	
	}
	return FVector::ZeroVector;
}

void AIPCharacterBase::HandleDeath(const FVector& DeathImpulse)
{
	// GetMesh()->SetSimulatePhysics(true);
	// GetMesh()->SetEnableGravity(true);
	// GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer DeathTags;
		DeathTags.AddTag(FIPGameplayTags::Get().Abilities_Death);
		AbilitySystemComponent->TryActivateAbilitiesByTag(DeathTags);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bDead = true;
	BleedingDebuffComponent->Deactivate();
	StunDebuffComponent->Deactivate();
	OnDeathDelegate.Broadcast(this);
}

void AIPCharacterBase::BeginPlay()
{
	Super::BeginPlay();


}

void AIPCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

#if WITH_EDITOR
void AIPCharacterBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, LeftCollisionBoxAttachBoneName))
	{
		LeftCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftCollisionBoxAttachBoneName);
	}

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, RightCollisionBoxAttachBoneName))
	{
		RightCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightCollisionBoxAttachBoneName);
	}

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, BodyCollisionBoxAttachBoneName))
	{
		BodyCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BodyCollisionBoxAttachBoneName);
	}
}
#endif

void AIPCharacterBase::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void AIPCharacterBase::OnBodyCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AIPCharacterBase::InitAbilityActorInfo()
{
}

void AIPCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float InLevel) const
{
	check(IsValid(GetAbilitySystemComponent()))
	check(GameplayEffectClass)

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, InLevel, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AIPCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AIPCharacterBase::AddCharacterAbilities()
{
	UIPAbilitySystemComponent* IPASC = CastChecked<UIPAbilitySystemComponent>(GetAbilitySystemComponent());
	IPASC->AddCharacterAbilities(StartupAbilities);
	IPASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}



