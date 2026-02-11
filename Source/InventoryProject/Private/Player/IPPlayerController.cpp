// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/IPPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "IPGameplayTags.h"
#include "AbilitySystem/IPAbilitySystemComponent.h"
#include "Characters/IPHeroCharacter.h"
#include "Components/IPInventorySystemComponent.h"
#include "Components/IPQuickSlotComponent.h"
#include "Input/IPInputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widgets/DamageTextComponent.h"
#include "UI/Widgets/IPUserWidget.h"


AIPPlayerController::AIPPlayerController()
{
}

void AIPPlayerController::ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit,
	bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass)
	{
	//	UE_LOG(LogTemp,Warning,TEXT("PC->ShowDamageNumber"));
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

void AIPPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(IPContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem)
	{
		Subsystem->AddMappingContext(IPContext, 0);
	}
}

void AIPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UIPInputComponent* IPInputComponent = CastChecked<UIPInputComponent>(InputComponent);

	IPInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIPPlayerController::Move);
	IPInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AIPPlayerController::Look);
	IPInputComponent->BindAction(PickUpAction, ETriggerEvent::Started, this, &AIPPlayerController::PickUp);
	IPInputComponent->BindAction(AttributeMenuAction, ETriggerEvent::Started, this, &AIPPlayerController::TurnOnAttributeMenuWidget);
	IPInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &AIPPlayerController::TurnOnInventoryWidget);
	IPInputComponent->BindAction(SkillMenuAction, ETriggerEvent::Started, this, &AIPPlayerController::TurnOnSkillMenuWidget);
	
	IPInputComponent->BindInventoryActions(InputConfig, this, &AIPPlayerController::UseItem);
	IPInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

UIPInventorySystemComponent* AIPPlayerController::GetISC()
{
	if (!CachedHeroCharacter.IsValid())
	{
		CachedHeroCharacter = Cast<AIPHeroCharacter> (GetPawn());
	}
	return CachedHeroCharacter->GetInventorySystemComponent();
}

UIPQuickSlotComponent* AIPPlayerController::GetQSC()
{
	if (!CachedHeroCharacter.IsValid())
	{
		CachedHeroCharacter = Cast<AIPHeroCharacter> (GetPawn());
	}
	return CachedHeroCharacter->GetQuickSlotComponent();
}

UIPAbilitySystemComponent* AIPPlayerController::GetASC()
{
	if (AbilitySystemComponent == nullptr)
	{
		AbilitySystemComponent = Cast<UIPAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AbilitySystemComponent;
}

void AIPPlayerController::Move(const FInputActionValue& InputActionValue)
{

	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f,  GetControlRotation().Yaw, 0.f);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		if (MovementVector.Y != 0.f)
		{
			const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		}

		if (MovementVector.X != 0.f)
		{
			const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
			ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
		}
	}
	
}

void AIPPlayerController::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		if (LookAxisVector.X != 0.f)
		{
			ControlledPawn->AddControllerYawInput(LookAxisVector.X);
		}

		if (LookAxisVector.Y != 0.f)
		{
			ControlledPawn->AddControllerPitchInput(LookAxisVector.Y);
		}
	}
}

void AIPPlayerController::PickUp(const FInputActionValue& InputActionValue)
{
	if (AIPHeroCharacter* ControlledCharacter = GetPawn<AIPHeroCharacter>())
	{
		ControlledCharacter->GetInventorySystemComponent()->PickUpItem();
		
	}
}

void AIPPlayerController::TurnOnAttributeMenuWidget(const FInputActionValue& InputActionValue)
{

	
	checkf(AttributeMenuWidgetClass, TEXT("AttributeMenuWidgetClass isn't set. Please set class in BP_IPPlayerController"));

	if (!bIsAttributeMenuWidgetTurnedOn)
	{

		if (AttributeMenuWidget == nullptr)
		{
			AttributeMenuWidget = CreateWidget<UIPUserWidget>(GetWorld(), AttributeMenuWidgetClass);
		}
		AttributeMenuWidget->AddToViewport();
		bIsAttributeMenuWidgetTurnedOn = true;
	}
	else
	{

		if (AttributeMenuWidget)
		{
			AttributeMenuWidget->RemoveFromParent();
		}
		bIsAttributeMenuWidgetTurnedOn = false;
	}
	SetInputModeBasedOnWidgets();
}

void AIPPlayerController::TurnOnInventoryWidget(const FInputActionValue& InputActionValue)
{
	checkf(InventoryWidgetClass, TEXT("InventoryWidgetClass isn't set. Please set class in BP_IPPlayerController"));

	if (!bIsInventoryWidgetTurnedOn)
	{

		if (InventoryWidget == nullptr)
		{
			InventoryWidget = CreateWidget<UIPUserWidget>(GetWorld(), InventoryWidgetClass);
		}
		InventoryWidget->AddToViewport();
		InventoryWidget->SetPositionInViewport(FVector2D(MenuPadding,MenuPadding), true);
		bIsInventoryWidgetTurnedOn = true;
	}
	else
	{

		if (InventoryWidget)
		{
			InventoryWidget->RemoveFromParent();
		}
		bIsInventoryWidgetTurnedOn = false;
	}
	SetInputModeBasedOnWidgets();
}

void AIPPlayerController::TurnOnSkillMenuWidget(const FInputActionValue& InputActionValue)
{
	if (!bIsSkillMenuWidgetTurnedOn)
	{

		if (SkillMenuWidget == nullptr)
		{
			SkillMenuWidget = CreateWidget<UIPUserWidget>(GetWorld(), SkillMenuWidgetClass);
		}
		SkillMenuWidget->AddToViewport();
		SkillMenuWidget->SetPositionInViewport(FVector2D(MenuPadding,MenuPadding), true);
		bIsSkillMenuWidgetTurnedOn = true;
	}
	else
	{

		if (SkillMenuWidget)
		{
			SkillMenuWidget->RemoveFromParent();
		}
		bIsSkillMenuWidgetTurnedOn = false;
	}
	SetInputModeBasedOnWidgets();
}

void AIPPlayerController::UseItem(FGameplayTag InputTag)
{
	if (InputTag == FIPGameplayTags::Get().InputTag_1)
	{
		if (GetQSC() && !GetQSC()->bIsEmptyIndex(0))
		{
			const int32& InventoryIndex = GetQSC()->GetQuickSlot(0).InventoryIndex;
			GetISC()->UseItem(InventoryIndex, 0);
		}
	}
	else if (InputTag == FIPGameplayTags::Get().InputTag_2)
	{

		if (GetQSC() && !GetQSC()->bIsEmptyIndex(1))
		{
			const int32& InventoryIndex = GetQSC()->GetQuickSlot(1).InventoryIndex;
			GetISC()->UseItem(InventoryIndex, 1);
		}
	}
	else if (InputTag == FIPGameplayTags::Get().InputTag_3)
	{

		if (GetQSC() && !GetQSC()->bIsEmptyIndex(2))
		{
			const int32& InventoryIndex = GetQSC()->GetQuickSlot(2).InventoryIndex;
			GetISC()->UseItem(InventoryIndex, 2);
		}
	}
	else if (InputTag == FIPGameplayTags::Get().InputTag_4)
	{

		if (GetQSC() && !GetQSC()->bIsEmptyIndex(3))
		{
			const int32& InventoryIndex = GetQSC()->GetQuickSlot(3).InventoryIndex;
			GetISC()->UseItem(InventoryIndex, 3);
		}
	}
}

void AIPPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC() && !GetASC()->HasMatchingGameplayTag(FIPGameplayTags::Get().Player_Block_InputPressed))
	{
		GetASC()->AbilityInputTagPressed(InputTag);
		return;
	}
}

void AIPPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() && !GetASC()->HasMatchingGameplayTag(FIPGameplayTags::Get().Player_Block_InputReleased))
	{
		GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}
	
}

void AIPPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC() && !GetASC()->HasMatchingGameplayTag(FIPGameplayTags::Get().Player_Block_InputHeld))
	{
		GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}

	
}

void AIPPlayerController::SetInputModeBasedOnWidgets()
{
	if (bIsAttributeMenuWidgetTurnedOn ||  bIsSkillMenuWidgetTurnedOn || bIsInventoryWidgetTurnedOn)
	{
		FInputModeGameAndUI InputModeGameAndUI;
		bShowMouseCursor = true;
		SetInputMode(InputModeGameAndUI);
	}
	else
	{
		FInputModeGameOnly InputModeGameOnly;
		bShowMouseCursor = false;
		SetInputMode(InputModeGameOnly);
	}
}
	
