// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetControllers/IPInventoryWidgetController.h"

#include "Components/IPInventorySystemComponent.h"
#include "Components/IPQuickSlotComponent.h"

void UIPInventoryWidgetController::BroadcastInitialValues()
{
	UIPInventoryWidgetController::BroadcastInventoryInfo();
}


void UIPInventoryWidgetController::BindCallbacksToDependencies()
{

	GetIPISC()->OnInventoryUpdatedDelegate.AddUObject(this, &UIPInventoryWidgetController::UpdateSlots);
	GetIPQSC()->OnQuickSlotUpdatedDelegate.AddUObject(this, &UIPInventoryWidgetController::UpdateQuickSlots);
	
	GetIPISC()->ItemsGivenDelegate.AddUObject(this, &UIPInventoryWidgetController::BroadcastInitialValues);
	GetIPQSC()->QuickSlotsGivenDelegate.AddUObject(this, &UIPInventoryWidgetController::BroadcastInitialValues);
}

void UIPInventoryWidgetController::BroadcastInventoryInfo()
{
	Super::BroadcastInventoryInfo();

	//Update함수바인드 한 델리게이트를 보내 ISC, QSC의 원소를 입력으로하는 함수를 브로드캐스트
	FForEachSlot BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FInventorySlot& InventorySlot, const int32& Index)
	{
		FIPItemInfo Info = ItemInfo->FindItemInfoForName(InventorySlot.ItemName);
		OnUpdateSlotDelegate.Broadcast(Info, Index, InventorySlot.Amount);
	}
	);

	FForEachQuickSlot QuickSlotBroadcastDelegate;
	QuickSlotBroadcastDelegate.BindLambda([this](const FQuickSlot& QuickSlot, const int32& Index)
	{
		FIPItemInfo Info = ItemInfo->FindItemInfoForName(QuickSlot.ItemName);//QuickSlot의 원소도 Amount를 가지고 있어야 한다.
		OnUpdateQuickSlotDelegate.Broadcast(Info, Index, QuickSlot.Amount);
	}
	);
	
	GetIPISC()->ForEachSlot(BroadcastDelegate);
	GetIPQSC()->ForEachQuickSlot(QuickSlotBroadcastDelegate);
}

void UIPInventoryWidgetController::OnDropped(const int32& TargetIndex, const int32& BeforeIndex)
{
	if (TargetIndex != BeforeIndex)
	{
		IPInventorySystemComponent->ChangeSlot(TargetIndex, BeforeIndex);
	}
}

void UIPInventoryWidgetController::OnDroppedOnQuickSlot(const int32& QuickSlotIndex, const int32& InventoryIndex)
{
	//UE_LOG(LogTemp,Warning, TEXT("OnDroppedOnQuickSlot Activated: InventoryIndex: %d, QuickSlotIndex: %d"),InventoryIndex+1, QuickSlotIndex+1);
	FInventorySlot InvenSlot = IPInventorySystemComponent->GetSlotElement(InventoryIndex);
	
	if (!IPQuickSlotComponent->bIsEmptyIndex(QuickSlotIndex)) //해당 인덱스의 퀵슬롯에 무엇인가 있을 때
	{
		FQuickSlot QuickSlot = IPQuickSlotComponent->GetQuickSlot(QuickSlotIndex);//해당 퀵 슬롯에 아이템이 존재할 경우에만 사용
		
		if (InvenSlot.ItemName == QuickSlot.ItemName) //해당 인덱스의 퀵슬롯에 같은 아이템이 있는 경우
		{
			
			if (QuickSlot.InventoryIndex != InventoryIndex) //퀵슬롯에 지정된 인벤토리 아이템의 인덱스가 바뀌면 해당 퀵슬롯의 인덱스도 변경해줘야 한다. 그 대신 더해주는 방향으로
			{
			//	UE_LOG(LogTemp,Warning, TEXT("Case 1"));
				//이미 있던 퀵슬롯에 넣는 것이므로 따로 퀵슬롯 인덱스 업데이트 없이 진행
				OnDropped(QuickSlot.InventoryIndex, InventoryIndex);
				//얘는 아래 Update슬롯에서 다 해결
			}
			//같은 인벤토리 인덱스면 아무것도 안 함
		}
		else //다른 아이템이면 아예 교체
		{
			
			FInventorySlot OriginalElement = IPInventorySystemComponent->GetSlotElement(QuickSlot.InventoryIndex);
			OriginalElement.QuickSlotIndex = -1;
			IPInventorySystemComponent->SetSlotElement(OriginalElement.InventoryIndex, OriginalElement);//퀵슬롯 인덱스 초기화

			//퀵슬롯 안에 같은 아이템이 있는지 체크
			
			int32 ExistingQuickSlotIndex = IPQuickSlotComponent->ExistingElementIndex(InvenSlot.ItemName, QuickSlotIndex);
			if (ExistingQuickSlotIndex == -1) //존재하지 않을 경우
			{
				//UE_LOG(LogTemp,Warning, TEXT("Case 2"));
				FQuickSlot NewQuickSlot;
				NewQuickSlot.ItemName = InvenSlot.ItemName;
				NewQuickSlot.InventoryIndex = InventoryIndex;
				NewQuickSlot.QuickSlotIndex = QuickSlotIndex;
				NewQuickSlot.Amount = InvenSlot.Amount;
				IPQuickSlotComponent->SetQuickSlot(QuickSlotIndex, NewQuickSlot);
			
				InvenSlot.QuickSlotIndex = QuickSlotIndex;
				IPInventorySystemComponent->SetSlotElement(InvenSlot.InventoryIndex, InvenSlot);
			}
			else //같은 원소가 퀵슬롯 내에 있으면 교체
			{
			//	UE_LOG(LogTemp,Warning, TEXT("Case 3"));
				//인벤토리 원소에 새로운 퀵슬롯 번호로 교체
				InvenSlot.QuickSlotIndex = QuickSlotIndex;
				IPInventorySystemComponent->SetSlotElement(InvenSlot.InventoryIndex, InvenSlot);

				//퀵슬롯 위치 바꾸기
				FQuickSlot EmptyQuickSlot;
				EmptyQuickSlot.ItemName = TEXT("None");
				EmptyQuickSlot.InventoryIndex = -1;
				EmptyQuickSlot.QuickSlotIndex = -1;
				EmptyQuickSlot.Amount = 0;
			
				FQuickSlot NewQuickSlot;
				NewQuickSlot.ItemName = InvenSlot.ItemName;
				NewQuickSlot.InventoryIndex = InventoryIndex;
				NewQuickSlot.QuickSlotIndex = QuickSlotIndex;
				NewQuickSlot.Amount = InvenSlot.Amount;
			
				IPQuickSlotComponent->SetQuickSlot(ExistingQuickSlotIndex, EmptyQuickSlot);
				IPQuickSlotComponent->SetQuickSlot(QuickSlotIndex, NewQuickSlot);
			}
			
			//퀵슬롯 데리게이트, SetQuickSlot함수 내에서 델리게이트 브로드캐스트하면 될 듯하다
		}
	}
	else //없을 때에는 퀵 슬롯 내에 같은 아이템이 있는지 확인, 없으면 바로 업데이트, 있으면 옮기기
	{
		int32 ExistingQuickSlotIndex = IPQuickSlotComponent->ExistingElementIndex(InvenSlot.ItemName, QuickSlotIndex);
		if (ExistingQuickSlotIndex == -1) //존재하지 않을 경우
		{
			//UE_LOG(LogTemp,Warning, TEXT("Case 4"));
			FQuickSlot NewQuickSlot;
			NewQuickSlot.ItemName = InvenSlot.ItemName;
			NewQuickSlot.InventoryIndex = InventoryIndex;
			NewQuickSlot.QuickSlotIndex = QuickSlotIndex;
			NewQuickSlot.Amount = InvenSlot.Amount;
			IPQuickSlotComponent->SetQuickSlot(QuickSlotIndex, NewQuickSlot);
			
			//UE_LOG(LogTemp, Warning, TEXT("InvenSlot-> Name:%s, InventorySlot: %d, QuickSlot: %d, Amount: %d"),*InvenSlot.ItemName.ToString(), InvenSlot.InventoryIndex, InvenSlot.QuickSlotIndex, InvenSlot.Amount);
			InvenSlot.QuickSlotIndex = QuickSlotIndex;
			IPInventorySystemComponent->SetSlotElement(InvenSlot.InventoryIndex, InvenSlot);
		}
		else //같은 원소가 퀵슬롯 내에 있으면 교체
		{
			//인벤토리 원소에 새로운 퀵슬롯 번호로 교체
			InvenSlot.QuickSlotIndex = QuickSlotIndex;
			IPInventorySystemComponent->SetSlotElement(InvenSlot.InventoryIndex, InvenSlot);
			//UE_LOG(LogTemp,Warning, TEXT("Case 5"));
			//퀵슬롯 위치 바꾸기
			FQuickSlot EmptyQuickSlot;
			EmptyQuickSlot.ItemName = TEXT("None");
			EmptyQuickSlot.InventoryIndex = -1;
			EmptyQuickSlot.QuickSlotIndex = -1;
			EmptyQuickSlot.Amount = 0;
			
			FQuickSlot NewQuickSlot;
			NewQuickSlot.ItemName = InvenSlot.ItemName;
			NewQuickSlot.InventoryIndex = InventoryIndex;
			NewQuickSlot.QuickSlotIndex = QuickSlotIndex;
			NewQuickSlot.Amount = InvenSlot.Amount;
			
			IPQuickSlotComponent->SetQuickSlot(ExistingQuickSlotIndex, EmptyQuickSlot);
			IPQuickSlotComponent->SetQuickSlot(QuickSlotIndex, NewQuickSlot);
		}
		//TODO: 퀵슬롯 위젯 업데이트시키는 델리게이트 브로드캐스트
	}
}

void UIPInventoryWidgetController::UpdateSlots(const FName& ItemID, const int32& InventoryIndex, const int32& QuickSlotIndex, const int32& Amount)
{
	OnUpdateSlotDelegate.Broadcast(ItemInfo->FindItemInfoForName(ItemID),InventoryIndex,Amount);

	//UE_LOG(LogTemp, Warning, TEXT("UpdateSlots Activated: Inventory Index: %d, Quick Index: %d"),InventoryIndex, QuickSlotIndex);
	FQuickSlot NewQuickSlot;
	NewQuickSlot.ItemName = ItemID;
	NewQuickSlot.InventoryIndex = InventoryIndex;
	NewQuickSlot.QuickSlotIndex = QuickSlotIndex;
	NewQuickSlot.Amount = Amount;
	
	IPQuickSlotComponent->SetQuickSlot(QuickSlotIndex, NewQuickSlot);
}

void UIPInventoryWidgetController::UpdateQuickSlots(const FName& ItemID, const int32& QuickSlotIndex,
	const int32& Amount)
{
	OnUpdateQuickSlotDelegate.Broadcast(ItemInfo->FindItemInfoForName(ItemID), QuickSlotIndex, Amount);
}


