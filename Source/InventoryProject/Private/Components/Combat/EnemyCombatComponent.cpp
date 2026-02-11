// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/EnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "IPGameplayTags.h"
#include "Abilities/GameplayAbilityTypes.h"

void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
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
}
