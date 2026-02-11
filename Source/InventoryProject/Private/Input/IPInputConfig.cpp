// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/IPInputConfig.h"

const UInputAction* UIPInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,
	bool bLogNotFound) const
{
	for (const FIPInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && InputTag == Action.InputTag)
		{
			return Action.InputAction;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find InputAction for InputTag [%s], on InputConfig [%s]"), *InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}
