// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IPInventorySystemLibrary.h"

#include "Characters/IPHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/IPHUD.h"
#include "UI/WidgetControllers/IPWidgetController.h"

bool UIPInventorySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject,
                                                           FWidgetControllerParams& OutWCParams, AIPHUD*& OutIPHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutIPHUD = Cast<AIPHUD>(PC->GetHUD());
		if (OutIPHUD)
		{
			if (AIPHeroCharacter* HeroCharacter = Cast<AIPHeroCharacter>(PC->GetPawn()))
			{
				OutWCParams.InventorySystemComponent = HeroCharacter->GetInventorySystemComponent();
				OutWCParams.QuickSlotComponent = HeroCharacter->GetQuickSlotComponent();
				OutWCParams.PlayerController = PC;
				return true;
			}
		}
	}
	return false;
}

UIPInventoryWidgetController* UIPInventorySystemLibrary::GetInventoryWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AIPHUD* IPHUD = nullptr;

	if (MakeWidgetControllerParams(WorldContextObject, WCParams, IPHUD))
	{
		return IPHUD->GetInventoryWidgetController(WCParams);
	}
	return nullptr;
}
