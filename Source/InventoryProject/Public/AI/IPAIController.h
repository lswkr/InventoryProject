// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "IPAIController.generated.h"

class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class INVENTORYPROJECT_API AIPAIController : public AAIController
{
	GENERATED_BODY()

public:
	AIPAIController();

protected:
	UPROPERTY()
	UBehaviorTreeComponent* BehaviorTreeComponent;

};
