// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "IPPlayerController.generated.h"


class UIPUserWidget;
class AIPHeroCharacter;
/**
 * 
 */
class UInputMappingContext;
class UInputAction;
class UIPAbilitySystemComponent;
class UIPInventorySystemComponent;
class UIPQuickSlotComponent;
class UIPInputConfig;
class UDamageTextComponent;

struct FInputActionValue;

UCLASS()
class INVENTORYPROJECT_API AIPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AIPPlayerController();

	UFUNCTION()
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	
private:
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* IPContext;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* PickUpAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AttributeMenuAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* InventoryAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SkillMenuAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UIPInputConfig* InputConfig;

	UPROPERTY()
	UIPInventorySystemComponent* InventorySystemComponent;
	
	UPROPERTY()
	UIPQuickSlotComponent* QuickSlotComponent;

	UPROPERTY()
	UIPAbilitySystemComponent* AbilitySystemComponent;
	
	UIPInventorySystemComponent* GetISC();
	UIPQuickSlotComponent* GetQSC();
	UIPAbilitySystemComponent* GetASC();
	
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void PickUp(const FInputActionValue& InputActionValue);
	void TurnOnAttributeMenuWidget(const FInputActionValue& InputActionValue);
	void TurnOnInventoryWidget(const FInputActionValue& InputActionValue);
	void TurnOnSkillMenuWidget(const FInputActionValue& InputActionValue);
	void UseItem(FGameplayTag InputTag);
	

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	UPROPERTY()
	TWeakObjectPtr<AIPHeroCharacter> CachedHeroCharacter;

	/* Widgets */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
	
	UPROPERTY()
	UIPUserWidget* AttributeMenuWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UIPUserWidget> AttributeMenuWidgetClass;

	UPROPERTY()
	UIPUserWidget* SkillMenuWidget;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UIPUserWidget> SkillMenuWidgetClass;

	UPROPERTY()
	UIPUserWidget* InventoryWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UIPUserWidget> InventoryWidgetClass;

	UPROPERTY(EditAnywhere)
	float MenuPadding = 25.f;
	
	bool bIsAttributeMenuWidgetTurnedOn;
	bool bIsSkillMenuWidgetTurnedOn;
	bool bIsInventoryWidgetTurnedOn;

	void SetInputModeBasedOnWidgets();
};
