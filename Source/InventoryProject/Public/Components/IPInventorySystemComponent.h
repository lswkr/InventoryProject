// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IPInventorySystemComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName ItemName = TEXT("None");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 Amount = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 InventoryIndex = -1;
	
	UPROPERTY(BlueprintReadWrite)
	int32 QuickSlotIndex = -1;
}
;


DECLARE_MULTICAST_DELEGATE_FourParams(FOnInventoryUpdated, const FName&, /*ItemID*/ const int32& /*InventoryIndex*/, const int32& /*QuickSlotIndex*/, const int32& /*Amount*/);
DECLARE_DELEGATE_TwoParams(FForEachSlot, const FInventorySlot&, const int32&);
DECLARE_MULTICAST_DELEGATE(FItemsGiven);

class ULoadScreenSaveGame;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYPROJECT_API UIPInventorySystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UIPInventorySystemComponent();

	int32 GetInventorySize() const {return Slots.Num();};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractionRange = 500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractionSphereRadius = 15.f;
	
	FOnInventoryUpdated OnInventoryUpdatedDelegate;
	
	UFUNCTION()
	void SetInventorySize(int32 InInventorySize);
	
	UFUNCTION()
	void PickUpItem();

	UPROPERTY(EditDefaultsOnly)
	UDataTable* ItemTable;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	bool bDrawDebugLine = false;

	UFUNCTION()
	void AddItem(const int32& ItemAmount, const FName& ItemKey);
	
	UFUNCTION()
	bool FindSlot(const FName& ItemKey, int32& SlotIndex);

	UFUNCTION()
	int32 GetStackSize(const FName& ItemKey) const;

	UFUNCTION()
	void IncreaseSlotStack(const int32& Index, const int32& Amount);

	UFUNCTION()
	bool CheckSlotEmpty(int32& EmptySlotIndex);

	UFUNCTION()
	void AddToNewSlot(const int32& Index, const FName& ItemKey, const int32& Amount);

	UFUNCTION()
	void ChangeSlot(const int32& TargetIndex, const int32& BeforeIndex);

	void ForEachSlot(const FForEachSlot& Delegate);

	FInventorySlot GetSlotElement(const int32& Index) const;
	void SetSlotElement(const int32& Index, FInventorySlot Slot);

	void UseItem(const int32& InventoryIndex, const int32& QuickSlotIndex);

	void ClearSlot(const int32& InventoryIndex);

	void AddCharacterItemsFromSaveData(ULoadScreenSaveGame* SaveData);

	FItemsGiven ItemsGivenDelegate;
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsAddFailed = false;

	// UFUNCTION(BlueprintImplementableEvent)
	// void OnSlotDropped();

	void InteractionTrace();

	UPROPERTY(BlueprintReadWrite)
	TArray<FInventorySlot> Slots;
	
private:
	UPROPERTY(EditAnywhere)
	USoundWave* PickUpSound;

	AActor* LookAtActor = nullptr;
	
	bool bIsAddFailed = false;
};
