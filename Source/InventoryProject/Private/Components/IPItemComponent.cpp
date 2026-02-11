// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/IPItemComponent.h"

#include "Characters/IPHeroCharacter.h"
#include "Components/IPInventorySystemComponent.h"

UIPItemComponent::UIPItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}



void UIPItemComponent::InteractWith_Implementation(AActor* PlayerCharacter)
{
	if (AIPHeroCharacter* HeroCharacter = Cast<AIPHeroCharacter>(PlayerCharacter))
	{
		HeroCharacter->GetInventorySystemComponent()->AddItem(ItemAmount, ItemID);
	}
	GetOwner()->Destroy();
}


void UIPItemComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}



