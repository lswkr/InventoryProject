// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IPWidgetController.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FIPAbilityInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInitialAbilityLevelSignature, const FGameplayTag&, AbilityTag,const int32&, Level);


class UIPInventorySystemComponent;
class UIPQuickSlotComponent;
class UItemInfo;
class AIPPlayerController;
class UAbilitySystemComponent;
class UAttributeSet;
class AIPPlayerState;
class UIPAbilitySystemComponent;
class UIPAttributeSet;
class UAbilityInfo;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() {}

	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS, UIPInventorySystemComponent* ISC, UIPQuickSlotComponent* QSC)
		:PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS), InventorySystemComponent(ISC), QuickSlotComponent(QSC){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlayerController* PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlayerState* PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAttributeSet* AttributeSet = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UIPInventorySystemComponent* InventorySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UIPQuickSlotComponent* QuickSlotComponent = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryInfoSignature, const FIPItemInfo&, Info, const int32&, Index);

UCLASS()
class INVENTORYPROJECT_API UIPWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();

	virtual void BroadcastInventoryInfo();
	virtual void BroadcastAbilityInfo();
	UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;
	
	UPROPERTY(BlueprintAssignable,Category = "GAS|Messages")
	FInitialAbilityLevelSignature InitialAbilityLevelDelegate;
	
	UPROPERTY(BlueprintAssignable,Category = "GAS|Messages")//메시지 용
	FInventoryInfoSignature InventoryInfoDelegate;
	
	UFUNCTION(BlueprintPure)
	int32 GetInventorySize();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	APlayerController* PlayerController;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	APlayerState* PlayerState ;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	UAttributeSet* AttributeSet;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	AIPPlayerController* IPPlayerController;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	AIPPlayerState* IPPlayerState ;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	UIPAbilitySystemComponent* IPAbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	UIPAttributeSet* IPAttributeSet;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	UIPInventorySystemComponent* IPInventorySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	UIPQuickSlotComponent* IPQuickSlotComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UItemInfo* ItemInfo;

	AIPPlayerController* GetIPPC();
	AIPPlayerState* GetIPPS();
	UIPAbilitySystemComponent* GetIPASC();
	UIPAttributeSet* GetIPAS();
	UIPInventorySystemComponent* GetIPISC();
	UIPQuickSlotComponent* GetIPQSC();
	
};
