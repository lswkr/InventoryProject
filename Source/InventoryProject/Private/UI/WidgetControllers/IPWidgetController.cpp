// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetControllers/IPWidgetController.h"

#include "MeshPassProcessor.h"
#include "AbilitySystem/IPAbilitySystemComponent.h"
#include "AbilitySystem/IPAbilitySystemLibrary.h"
#include "AbilitySystem/IPAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Components/IPInventorySystemComponent.h"
#include "Components/IPQuickSlotComponent.h"
#include "Items/ItemInfo.h"
#include "Player/IPPlayerController.h"
#include "Player/IPPlayerState.h"

void UIPWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
	IPInventorySystemComponent = WCParams.InventorySystemComponent;
	IPQuickSlotComponent = WCParams.QuickSlotComponent;
}

void UIPWidgetController::BroadcastInitialValues()
{
}

void UIPWidgetController::BindCallbacksToDependencies()
{
}

void UIPWidgetController::BroadcastInventoryInfo() //위젯 켤 때
{
	
}

void UIPWidgetController::BroadcastAbilityInfo()
{
	if (!GetIPASC()->bStartupAbilitiesGiven) return;
	FForEachAbility BroadcastDelegate; 
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FIPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(IPAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = IPAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
		Info.StatusTag = IPAbilitySystemComponent->GetStatusFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
		//UE_LOG(LogTemp, Warning, TEXT("%s broadcast function binded. STATUS: %s"), *Info.AbilityTag.ToString(),*Info.StatusTag.ToString());
	});
	
	GetIPASC()->ForEachAbility(BroadcastDelegate);
}

int32 UIPWidgetController::GetInventorySize() 
{
	return GetIPISC()->GetInventorySize();
}


AIPPlayerController* UIPWidgetController::GetIPPC()
{
	if (IPPlayerController == nullptr)
	{
		IPPlayerController = Cast<AIPPlayerController>(PlayerController);
	}
	return IPPlayerController;
}

AIPPlayerState* UIPWidgetController::GetIPPS()
{
	if (IPPlayerState == nullptr)
	{
		IPPlayerState = Cast<AIPPlayerState>(PlayerState);
	}
	return IPPlayerState;
}

UIPAbilitySystemComponent* UIPWidgetController::GetIPASC()
{
	if (IPAbilitySystemComponent == nullptr)
	{
		IPAbilitySystemComponent = Cast<UIPAbilitySystemComponent>(AbilitySystemComponent);
	}
	return IPAbilitySystemComponent;
}

UIPAttributeSet* UIPWidgetController::GetIPAS()
{
	if (IPAttributeSet == nullptr)
	{
		IPAttributeSet = Cast<UIPAttributeSet>(AttributeSet);
	}
	return IPAttributeSet;
}

UIPInventorySystemComponent* UIPWidgetController::GetIPISC()
{
	return IPInventorySystemComponent;
}

UIPQuickSlotComponent* UIPWidgetController::GetIPQSC()
{
	return IPQuickSlotComponent;
}
