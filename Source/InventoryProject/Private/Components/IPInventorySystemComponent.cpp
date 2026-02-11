// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/IPInventorySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystem/IPAbilitySystemLibrary.h"
#include "Components/IPItemComponent.h"
#include "Game/LoadScreenSaveGame.h"
#include "Interfaces/InventoryInterface.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryProject/InventoryProject.h"
#include "Items/ItemInfo.h"


UIPInventorySystemComponent::UIPInventorySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UIPInventorySystemComponent::SetInventorySize(int32 InInventorySize)
{
	Slots.SetNum(InInventorySize);
}

void UIPInventorySystemComponent::PickUpItem()
{
	if (LookAtActor)
	{
		if (UActorComponent* ItemComponent = LookAtActor->GetComponentByClass(UIPItemComponent::StaticClass()))
		{
			IInventoryInterface::Execute_InteractWith(ItemComponent, GetOwner());
			if (PickUpSound !=nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(
				this, 
				PickUpSound, 
				GetOwner()->GetActorLocation()
				);
			}
		}

	}
}

void UIPInventorySystemComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	InteractionTrace();
}

void UIPInventorySystemComponent::AddItem(const int32& ItemAmount, const FName& ItemKey)
{
	int32 AmountLeft = ItemAmount;

	while ( !bIsAddFailed && (AmountLeft > 0 ))
	{
		int32 SlotIndex = -1;

		bool FoundSlot = FindSlot(ItemKey, SlotIndex);

		if (FoundSlot) //같은 아이템이 있을 때
		{
			//UE_LOG(LogTemp, Warning, TEXT("AddItem Case1: SlotIndex: %d"), SlotIndex);
			IncreaseSlotStack(SlotIndex, 1);
			OnInventoryUpdatedDelegate.Broadcast(ItemKey, SlotIndex, Slots[SlotIndex].QuickSlotIndex, Slots[SlotIndex].Amount);
			AmountLeft--;
		}
		else if (CheckSlotEmpty(SlotIndex)) //빈 곳 아무 곳이나
		{
			//UE_LOG(LogTemp, Warning, TEXT("AddItem Case2: SlotIndex: %d"), SlotIndex);
			AddToNewSlot(SlotIndex, ItemKey, 1);
			OnInventoryUpdatedDelegate.Broadcast(ItemKey, SlotIndex, Slots[SlotIndex].QuickSlotIndex, Slots[SlotIndex].Amount);
			AmountLeft--;
		}
		else //꽉 찼을 때
		{
			bIsAddFailed = true;
		}
		
	}
}

bool UIPInventorySystemComponent::FindSlot(const FName& ItemKey, int32& SlotIndex)
{
	for (int32 i=0 ; i<Slots.Num();i++)
	{
		if (Slots[i].ItemName == ItemKey)
		{
			if (Slots[i].Amount < GetStackSize(ItemKey))
			{
				SlotIndex = i;
				return true;
			}
		}
	}
	
	SlotIndex = -1;
	return false;
}

int32 UIPInventorySystemComponent::GetStackSize(const FName& ItemKey) const
{
	FIPItemStruct* Slot = ItemTable->FindRow<FIPItemStruct>(ItemKey, TEXT(""));
	UItemInfo* ItemInformation = UIPAbilitySystemLibrary::GetItemInfo(GetOwner());

	FIPItemInfo Info = ItemInformation->FindItemInfoForName(ItemKey);
	
	if (Info.IsValid())
	{
		return Info.StackSize;
	}
	else
	{
		return -1;
	}
}

void UIPInventorySystemComponent::IncreaseSlotStack(const int32& Index, const int32& Amount)
{
	FInventorySlot NewSlot;

	NewSlot.Amount = Slots[Index].Amount + Amount;
	NewSlot.ItemName = Slots[Index].ItemName;
	NewSlot.InventoryIndex = Index;
	NewSlot.QuickSlotIndex = Slots[Index].QuickSlotIndex;
	Slots[Index] = NewSlot;
}

bool UIPInventorySystemComponent::CheckSlotEmpty(int32& EmptySlotIndex)
{
	for (int32 i=0;i<Slots.Num();i++)
	{
		if (Slots[i].Amount ==0)
		{
			EmptySlotIndex = i;
			return true;
		}
		
	}
	EmptySlotIndex = -1;
	return false;
}

void UIPInventorySystemComponent::AddToNewSlot(const int32& Index, const FName& ItemKey, const int32& Amount) //처음 먹을 때 사용하는 함수. 퀵슬롯은 따로 업데이트 할 필요가 없다.
{
	Slots[Index].ItemName = ItemKey;
	Slots[Index].Amount = Amount;
	Slots[Index].InventoryIndex = Index;
//	UE_LOG(LogTemp, Warning, TEXT("AddToNewSlot Activated: Index: %d, Slots[Index]: %d" ), Index, Slots[Index].InventoryIndex);
}

void UIPInventorySystemComponent::ChangeSlot(const int32& TargetIndex, const int32& BeforeIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("ChangeSlot Activated: TargetIndex: %d, BeforeIndex: %d"), TargetIndex+1, BeforeIndex+1);
	if (TargetIndex>=0)
	{
		if (Slots[TargetIndex].ItemName == TEXT("None")) //비어있는 경우
		{
	//		UE_LOG(LogTemp, Warning, TEXT("Case1"));

			FInventorySlot NewSlot;
			NewSlot.ItemName = Slots[BeforeIndex].ItemName;
			NewSlot.InventoryIndex = TargetIndex;
			NewSlot.QuickSlotIndex = Slots[BeforeIndex].QuickSlotIndex;
			NewSlot.Amount = Slots[BeforeIndex].Amount;
			SetSlotElement(TargetIndex, NewSlot);
			//UE_LOG(LogTemp, Warning, TEXT("Slots[BeforeIndex]: Inven: %d, Quick: %d"), Slots[BeforeIndex].InventoryIndex, Slots[BeforeIndex].QuickSlotIndex);

			ClearSlot(BeforeIndex);
			OnInventoryUpdatedDelegate.Broadcast(Slots[BeforeIndex].ItemName, BeforeIndex, Slots[BeforeIndex].QuickSlotIndex, Slots[BeforeIndex].Amount);
		}
		else if (Slots[TargetIndex].ItemName != Slots[BeforeIndex].ItemName) //아이템이 다를 경우
		{
			//UE_LOG(LogTemp, Warning, TEXT("Case2"));
			FInventorySlot BeforeSlot = Slots[BeforeIndex];
			FInventorySlot TargetSlot = Slots[TargetIndex];
			BeforeSlot.InventoryIndex = TargetIndex;
			TargetSlot.InventoryIndex = BeforeIndex;
			
			Slots[TargetIndex] = BeforeSlot;
			Slots[BeforeIndex] = TargetSlot;
			
			OnInventoryUpdatedDelegate.Broadcast(Slots[BeforeIndex].ItemName,Slots[BeforeIndex].InventoryIndex , Slots[BeforeIndex].QuickSlotIndex, Slots[BeforeIndex].Amount);
			OnInventoryUpdatedDelegate.Broadcast(Slots[TargetIndex].ItemName,Slots[TargetIndex].InventoryIndex, Slots[TargetIndex].QuickSlotIndex, Slots[TargetIndex].Amount);
		}
		else //아이템이 같을 경우
		{
			
			int32 AmountOfTwoSlots = Slots[TargetIndex].Amount + Slots[BeforeIndex].Amount;
			int32 SizeLimit =  GetStackSize(Slots[TargetIndex].ItemName);
			if (AmountOfTwoSlots > GetStackSize(Slots[TargetIndex].ItemName))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Case3"));
				Slots[TargetIndex].Amount = SizeLimit;
				Slots[BeforeIndex].Amount = AmountOfTwoSlots-SizeLimit;
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("Case4"));
				Slots[TargetIndex].Amount= AmountOfTwoSlots;
				ClearSlot(BeforeIndex);
			}
			OnInventoryUpdatedDelegate.Broadcast(Slots[BeforeIndex].ItemName, BeforeIndex, Slots[BeforeIndex].QuickSlotIndex, Slots[BeforeIndex].Amount);
			OnInventoryUpdatedDelegate.Broadcast(Slots[TargetIndex].ItemName, TargetIndex, Slots[TargetIndex].QuickSlotIndex, Slots[TargetIndex].Amount);
		}
		
	}
}

void UIPInventorySystemComponent::ForEachSlot(const FForEachSlot& Delegate)
{
	for (int32 i=0; i<Slots.Num();i++)
	{
		
		if (!Delegate.ExecuteIfBound(Slots[i],i))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed To Execute Delegate in %hs"),__FUNCTION__);
		}
	}
}

FInventorySlot UIPInventorySystemComponent::GetSlotElement(const int32& Index) const
{
	return Slots[Index];
}

void UIPInventorySystemComponent::SetSlotElement(const int32& Index, FInventorySlot Slot)
{
	if (Index<0) return;
	
	Slots[Index] = Slot;
	//UE_LOG(LogTemp, Warning, TEXT("SetSlotElement: QuickSlot: %d"),Slots[Index].QuickSlotIndex);
	OnInventoryUpdatedDelegate.Broadcast(Slots[Index].ItemName, Index, Slots[Index].QuickSlotIndex, Slots[Index].Amount);
}

void UIPInventorySystemComponent::UseItem(const int32& InventoryIndex, const int32& QuickSlotIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("UseItem Activated: InventoryIndex: %d, QuickSlotIndex: %d"),InventoryIndex+1, QuickSlotIndex+1);
	
	if (Slots[InventoryIndex].Amount <= 0) return;
	
	Slots[InventoryIndex].Amount--;
	UItemInfo* ItemInfo = UIPAbilitySystemLibrary::GetItemInfo(GetOwner());

	FIPItemInfo Info = ItemInfo->FindItemInfoForName(Slots[InventoryIndex].ItemName);


	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());

	if (TargetASC)
	{
		check(Info.ItemEffect);

		//UE_LOG(LogTemp, Warning, TEXT("Using Item"));
		FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(Info.ItemEffect, 1, EffectContextHandle);
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
	
	
	if (Slots[InventoryIndex].Amount == 0)//InventorySlot의 QuickSlot 인덱스는 초기화하되, QuickSlot에 접근할 인덱스는 업데이트 해줘야하므로 -1로 만들면 안 된다.
	{
		ClearSlot(InventoryIndex);
	}

	
	OnInventoryUpdatedDelegate.Broadcast(Slots[InventoryIndex].ItemName, InventoryIndex, QuickSlotIndex, Slots[InventoryIndex].Amount);
}

void UIPInventorySystemComponent::ClearSlot(const int32& InventoryIndex)
{
	Slots[InventoryIndex].ItemName = TEXT("None");
	Slots[InventoryIndex].Amount = 0;
	Slots[InventoryIndex].QuickSlotIndex = -1;
	Slots[InventoryIndex].InventoryIndex = -1;
}


void UIPInventorySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UIPInventorySystemComponent::InteractionTrace()
{
	FHitResult HitResult;
	FCollisionQueryParams Params;
	FVector StartLocation = UGameplayStatics::GetPlayerCameraManager(GetOwner(), 0)->GetCameraLocation();
	FVector ForwardVector = UGameplayStatics::GetPlayerCameraManager(GetOwner(), 0)->GetActorForwardVector();	
	FVector EndLocation = StartLocation + ForwardVector * InteractionRange;
	Params.AddIgnoredActor(GetOwner());
	
	bool HitDetected = GetWorld()->SweepSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_Interaction,
		FCollisionShape::MakeSphere(InteractionSphereRadius),
		Params	
	);

	if (HitDetected)
	{
		if (!LookAtActor || LookAtActor != HitResult.GetActor())
		{
			LookAtActor = HitResult.GetActor();
			
		}

		if (LookAtActor->Implements<UInventoryInterface>())
		{
			IInventoryInterface::Execute_LookAt(LookAtActor);
		}
	}
	else
	{
		LookAtActor = nullptr;
	}
}

void UIPInventorySystemComponent::AddCharacterItemsFromSaveData(ULoadScreenSaveGame* SaveData)
{
	//UE_LOG(LogTemp, Warning, TEXT("AddCharacterAbilitiesFromSaveData Activated"));
	if (Slots.Num()==0)
	{
	//	UE_LOG(LogTemp,Warning, TEXT("Slots is 0"));
		return;
	}
	for (const FSavedItem& Data : SaveData->SavedItems)
	{
	//	UE_LOG(LogTemp, Warning, TEXT("SavedItem: %s, Index: %d"),*Data.ItemName.ToString(), Data.Index);
		if (Data.Index<0) continue;
		
		Slots[Data.Index].ItemName = Data.ItemName;
		Slots[Data.Index].Amount = Data.Amount;
		Slots[Data.Index].InventoryIndex = Data.Index;
		Slots[Data.Index].QuickSlotIndex = Data.QuickSlotIndex;
	}
	ItemsGivenDelegate.Broadcast();
}

