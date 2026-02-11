// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "IPInputConfig.h"
#include "IPInputComponent.generated.h"

/**
 * 
 */


UCLASS()
class INVENTORYPROJECT_API UIPInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	template<class UserClass, typename InventoryFuncType>
	void BindInventoryActions(const UIPInputConfig* InputConfig, UserClass* Object, InventoryFuncType InventoryFunc);

	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UIPInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

template <class UserClass, typename InventoryFuncType>
void UIPInputComponent::BindInventoryActions(const UIPInputConfig* InputConfig, UserClass* Object,
	InventoryFuncType InventoryFunc)
{
	check(InputConfig);

	for (const FIPInputAction& Action : InputConfig->InventoryInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (InventoryFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, InventoryFunc, Action.InputTag);
			}
		}
	}
}


template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UIPInputComponent::BindAbilityActions(const UIPInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (const FIPInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}

			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
			
			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
	}
}
