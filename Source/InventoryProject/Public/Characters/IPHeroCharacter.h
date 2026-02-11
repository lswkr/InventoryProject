// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/IPCharacterBase.h"
#include "Interfaces/PlayerInterface.h"
#include "IPHeroCharacter.generated.h"

class UHeroCombatComponent;
/**
 * 
 */
class UCameraComponent;
class USpringArmComponent;
class UIPInventorySystemComponent;
class UIPQuickSlotComponent;

UCLASS()
class INVENTORYPROJECT_API AIPHeroCharacter : public AIPCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AIPHeroCharacter();
	virtual void PossessedBy(AController* NewController) override;

	/* Player Interface */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;
	virtual int32 GetSkillPointsReward_Implementation(int32 Level) const override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual void AddToSkillPoints_Implementation(int32 InSkillPoints) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSkillPoints_Implementation() const override;
	virtual void SaveProgress_Implementation(const FName& CheckpointTag) override;
	virtual UAnimMontage* GetHitReactMontage_Implementation(int32 Index) override;
	/* end Player Interface */

	/* Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual UPawnCombatComponent* GetPawnCombatComponent_Implementation() override;
	/* end Combat Interface */

	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;

	FTimerHandle DeathTimer;

	FORCEINLINE UIPInventorySystemComponent* GetInventorySystemComponent() { return IPInventorySystemComponent; }
	FORCEINLINE UIPQuickSlotComponent* GetQuickSlotComponent() { return IPQuickSlotComponent; }

	void LoadProgress();

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	virtual void OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                            const FHitResult& SweepResult) override;
	virtual void OnBodyCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;

private:
	UPROPERTY(EditDefaultsOnly)
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly)
	int32 InventorySize = 16;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	UIPInventorySystemComponent* IPInventorySystemComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	UIPQuickSlotComponent* IPQuickSlotComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UHeroCombatComponent* HeroCombatComponent;

	UPROPERTY(EditAnywhere, Category = "Character Class Defaults")
	FRotator BaseRotationRate = FRotator(0.f, 500.f, 0.f);

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> RegenerationEffect;

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimMontage*> HitReactMontages;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystemComponent* LevelUpParticleComponent;
};
