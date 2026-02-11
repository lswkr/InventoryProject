// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/IPQuickSlotComponent.h"

#include "Game/LoadScreenSaveGame.h"

UIPQuickSlotComponent::UIPQuickSlotComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FQuickSlot UIPQuickSlotComponent::GetQuickSlot(const int32& Index) const
{
	return QuickSlots[Index];
}

void UIPQuickSlotComponent::SetQuickSlot(const int32& Index, FQuickSlot QuickSlot)
{
	if (Index<0) return;
	QuickSlots[Index].ItemName = QuickSlot.ItemName;
	QuickSlots[Index].InventoryIndex = QuickSlot.InventoryIndex;
	QuickSlots[Index].QuickSlotIndex = QuickSlot.QuickSlotIndex;
	QuickSlots[Index].Amount = QuickSlot.Amount;

	OnQuickSlotUpdatedDelegate.Broadcast(QuickSlot.ItemName, Index, QuickSlot.Amount);
}

bool UIPQuickSlotComponent::bIsEmptyIndex(const int32& Index) const
{
	if (QuickSlots[Index].ItemName == "" || QuickSlots[Index].ItemName == "None")
	{
		//UE_LOG(LogTemp, Warning, TEXT("%d Is Empty"), Index);
		return true;
	}
	return false;
}

int32 UIPQuickSlotComponent::ExistingElementIndex(const FName& InItemName, const int32& NewIndex)
{
	for (int32 i=0; i<QuickSlots.Num(); i++)
	{
		if (NewIndex == i) continue;
		if (QuickSlots[i].ItemName == InItemName)
			return i;
	}
	return -1;
}

void UIPQuickSlotComponent::ForEachQuickSlot(const FForEachQuickSlot& Delegate)
{
	for (int32 i=0; i<QuickSlots.Num();i++)
	{
		if (!Delegate.ExecuteIfBound(QuickSlots[i],i))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed To Execute Delegate in %hs"),__FUNCTION__);
		}
	}
}

void UIPQuickSlotComponent::ClearSlot(const int32& Index)
{
	QuickSlots[Index].ItemName = TEXT("None");
	QuickSlots[Index].QuickSlotIndex = -1;
	QuickSlots[Index].Amount = 0;
}


void UIPQuickSlotComponent::BeginPlay()
{
	Super::BeginPlay();
	if (QuickSlots.Num() == 0)
		QuickSlots.SetNum(4); //퀵 슬롯 4개 고정
}



void UIPQuickSlotComponent::AddCharacterQuickSlotsFromSaveData(ULoadScreenSaveGame* SaveData)
{

	if (QuickSlots.Num()==0)
	{
		QuickSlots.SetNum(4);
	}
	for (const FSavedQuickSlot& Data : SaveData->SavedQuickSlots)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Load SavedItem: %s, Amount: %d"),*Data.ItemName.ToString(), Data.Amount);
		if (Data.QuickSlotIndex<0) continue;
		
		QuickSlots[Data.QuickSlotIndex].ItemName = Data.ItemName;
		QuickSlots[Data.QuickSlotIndex].Amount = Data.Amount;
		QuickSlots[Data.QuickSlotIndex].InventoryIndex = Data.InventoryIndex;
		QuickSlots[Data.QuickSlotIndex].QuickSlotIndex = Data.QuickSlotIndex;
		
	}
	QuickSlotsGivenDelegate.Broadcast();
}
