// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IPInventorySystemLibrary.generated.h"

class AIPHUD;
class UIPInventoryWidgetController;
/**
 * 
 */
UCLASS()
class INVENTORYPROJECT_API UIPInventorySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*
	 * Widget Controller
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AIPHUD*& OutIPHUD);

	
	UFUNCTION(BlueprintPure, Category = "Inventory|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UIPInventoryWidgetController* GetInventoryWidgetController(const UObject* WorldContextObject);	
	
};
