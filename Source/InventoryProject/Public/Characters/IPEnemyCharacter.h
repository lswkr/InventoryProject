// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/IPCharacterBase.h"
#include "Interfaces/EnemyInterface.h"
#include "UI/WidgetControllers/OverlayWidgetController.h"
#include "IPEnemyCharacter.generated.h"

class UEnemyCombatComponent;
/**
 * 
 */
class UWidgetComponent;
class AIPAIController;
class UBehaviorTree;
UCLASS()
class INVENTORYPROJECT_API AIPEnemyCharacter : public AIPCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
public:
	AIPEnemyCharacter();
	virtual void PossessedBy(AController* NewController) override;

	/** Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual UPawnCombatComponent* GetPawnCombatComponent_Implementation() override;
	/** end Combat Interface */

	/** Enemy Interface */
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual TArray<UAnimMontage*> GetAttackMontages_Implementation() override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual UAnimMontage* GetDeathMontage_Implementation() override;
	virtual FProjectileInfo GetProjectileInfo_Implementation() const override;
	/** End Enemy Interface */
	
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;

	virtual void OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	UPROPERTY(EditAnywhere, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Melee;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY()
	AIPAIController* AIController;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere,Category = "Montage")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere,Category = "Montage")
	UAnimMontage* DeathMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UEnemyCombatComponent* EnemyCombatComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FProjectileInfo ProjectileInfo;
};
