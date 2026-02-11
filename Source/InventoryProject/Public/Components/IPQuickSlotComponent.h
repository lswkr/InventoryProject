// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IPQuickSlotComponent.generated.h"

class ULoadScreenSaveGame;

USTRUCT(BlueprintType)
struct FQuickSlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName ItemName = TEXT("None");

	UPROPERTY(BlueprintReadWrite)
	int32 InventoryIndex = -1;
	
	UPROPERTY(BlueprintReadWrite)
	int32 QuickSlotIndex = -1;

	UPROPERTY(BlueprintReadWrite)
	int32 Amount = 0;
	
};


DECLARE_DELEGATE_TwoParams(FForEachQuickSlot, const FQuickSlot&, const int32&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnQuickSlotUpdated, const FName& /*ItemID*/, const int32& /*QuickSlotIndex*/, const int32& /*Amount*/);
DECLARE_MULTICAST_DELEGATE(FQuickSlotsGiven);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYPROJECT_API UIPQuickSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UIPQuickSlotComponent();

	FQuickSlot GetQuickSlot(const int32& Index) const;
	void SetQuickSlot(const int32& Index, FQuickSlot QuickSlot);
	bool bIsEmptyIndex(const int32& Index) const;
	int32 ExistingElementIndex(const FName& InItemName, const int32& NewIndex);
	
	FOnQuickSlotUpdated OnQuickSlotUpdatedDelegate;
	FQuickSlotsGiven QuickSlotsGivenDelegate;
	void ForEachQuickSlot(const FForEachQuickSlot& Delegate);
	void ClearSlot(const int32& Index);

	void AddCharacterQuickSlotsFromSaveData(ULoadScreenSaveGame* SaveData);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<FQuickSlot> QuickSlots;

	

	
};
