// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ItemInfo.h"
#include "UI/WidgetControllers/IPWidgetController.h"
#include "IPInventoryWidgetController.generated.h"

/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetItem, FName, Name,int32, Amount, int32, Index);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUpdateSlot, const FIPItemInfo&, ItemInfo,const int32&, Index, const int32&, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUpdateQuickSlot, const FIPItemInfo&, ItemInfo,const int32&, Index, const int32&, Amount);


UCLASS(Blueprintable)
class INVENTORYPROJECT_API UIPInventoryWidgetController : public UIPWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValues() override;
    virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInventoryInfo() override;
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory") 
	FOnGetItem OnGetItem;
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnUpdateSlot OnUpdateSlotDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnUpdateQuickSlot OnUpdateQuickSlotDelegate;
	
	
	UFUNCTION(BlueprintCallable)
	void OnDropped(const int32& TargetIndex, const int32& BeforeIndex);

	UFUNCTION(BlueprintCallable)
	void OnDroppedOnQuickSlot(const int32& QuickSlotIndex, const int32& InventoryIndex);

	UFUNCTION(BlueprintCallable)
	void UpdateSlots(const FName& ItemID, const int32& InventoryIndex, const int32& QuickSlotIndex,const int32& Amount);

	UFUNCTION(BlueprintCallable)
	void UpdateQuickSlots(const FName& ItemID, const int32& QuickSlotIndex,const int32& Amount);

	void SetInventorySlot();
	void SetQuickSlot();

};


