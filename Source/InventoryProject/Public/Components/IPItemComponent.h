// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/InventoryInterface.h"
#include "IPItemComponent.generated.h"

class UItemInfo;

USTRUCT(BlueprintType)
struct FIPItemStruct : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name = TEXT("None");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description = TEXT("None");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StackSize = 0;
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYPROJECT_API UIPItemComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

public:	
	UIPItemComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* ItemDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UItemInfo* ItemInfo;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemAmount = 1;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FName ItemID;

	virtual void InteractWith_Implementation(AActor* PlayerCharacter) override;
	
protected:
	virtual void BeginPlay() override;

	

};
