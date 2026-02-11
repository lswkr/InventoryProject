// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemInfo.h"

FIPItemInfo UItemInfo::FindItemInfoForName(const FName& ItemID) const
{
	for (const FIPItemInfo& ItemInfo : ItemInformation)
	{
		if (ItemID==ItemInfo.ItemID)
		{
			return ItemInfo;
		}
	}
	return FIPItemInfo();
}
