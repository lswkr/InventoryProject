// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/HeroCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "IPGameplayTags.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Characters/IPHeroCharacter.h"
#include "Components/BoxComponent.h"

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}
	 
	OverlappedActors.AddUnique(HitActor);

	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;

	UE_LOG(LogTemp, Warning, TEXT("Hit Actor is [%s]"), *HitActor->GetName())
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor( 
		GetOwningPawn(),
		FIPGameplayTags::Get().Event_Shared_Hit,
		Data
	);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		FIPGameplayTags::Get().Event_Player_HitPause,
		FGameplayEventData()
	);
}

void UHeroCombatComponent::OnHitBoxPulledFromTargetActor(AActor* HitActor)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
	GetOwningPawn(),
	FIPGameplayTags::Get().Event_Player_HitPause,
	FGameplayEventData()
);
}

// void UHeroCombatComponent::ToggleBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
// {
// 	AIPCharacterBase* OwningCharacter = GetOwningPawn<AIPCharacterBase>();
//
// 	check(OwningCharacter);
//
// 	UBoxComponent* LeftCollisionBox = OwningCharacter->GetLeftCollisionBox();
// 	UBoxComponent* RightCollisionBox = OwningCharacter->GetRightCollisionBox();
// 	UBoxComponent* BodyCollisionBox = OwningCharacter->GetBodyCollisionBox();
// 	switch (ToggleDamageType)
// 	{
// 	case EToggleDamageType::Left:
// 		LeftCollisionBox->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
// 		break;
//
// 	case EToggleDamageType::Right:
// 		RightCollisionBox->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
// 		break;
// 		
// 	case EToggleDamageType::Body:
// 		BodyCollisionBox->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
// 		break;
// 		
// 	default:
// 		break;
// 	}
// 	if (!bShouldEnable)
// 	{
// 		OverlappedActors.Empty();
// 	}
// }
//
// void UHeroCombatComponent::ToggleSpecialAttackBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
// {
// 	AIPHeroCharacter* OwningCharacter = GetOwningPawn<AIPHeroCharacter>();
//
// 	check(OwningCharacter);
//
// 	UBoxComponent* LeftCollisionBox = OwningCharacter->GetLeftCollisionBox();
// 	UBoxComponent* RightCollisionBox = OwningCharacter->GetRightCollisionBox();
// 	UBoxComponent* BodyCollisionBox = OwningCharacter->GetBodyCollisionBox();
//
// 	/* ANS_ToggleCollision할 때, CollisionBox의 콜리전이 꺼지지 않은 채로 
// 	 * 캔슬되어 스페셜 무브가 발동되면 OverlappedActors에 Actor들이 남아있을 수 있어
// 	 * 제대로 데미지를 줄 수 없기 때문에 한 번 비워줘야 한다.
// 	 */
// 	if (bShouldEnable)
// 	{
// 		OverlappedActors.Empty();
// 	}
// 	switch (ToggleDamageType)
// 	{
// 	case EToggleDamageType::Left:
// 		LeftCollisionBox->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
// 		break;
//
// 	case EToggleDamageType::Right:
// 		RightCollisionBox->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
// 		break;
// 		
// 	case EToggleDamageType::Body:
// 		BodyCollisionBox->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
// 		break;
// 		
// 	default:
// 		break;
// 	}
// 	
// 	if (!bShouldEnable)
// 	{
// 		OverlappedActors.Empty();
// 	}
// }
