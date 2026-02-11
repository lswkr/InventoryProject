// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IPSaveGate.generated.h"

class UBoxComponent;
class URectLightComponent;

UCLASS()
class INVENTORYPROJECT_API AIPSaveGate : public AActor
{
	GENERATED_BODY()
	
public:	
	AIPSaveGate();
	
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;

	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;

	void EnableCollision(bool bShouldEnable);

	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* StaticMesh;
	
private:
	UPROPERTY(EditDefaultsOnly)
	URectLightComponent* RectLight;

	UPROPERTY(EditAnywhere)
	float SinePeriodConstant = 1.f;

	UPROPERTY(EditAnywhere)
	float BrightnessAmplitude = 10000.f;

	
	float RunningTime = 0;
	void LightGlowing(float DeltaTime);

	bool bTurnOnLight = false;
};
