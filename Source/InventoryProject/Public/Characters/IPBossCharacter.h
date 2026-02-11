// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/IPCharacterBase.h"
#include "Interfaces/BossInterface.h"
#include "Interfaces/EnemyInterface.h"
#include "UI/WidgetControllers/OverlayWidgetController.h"
#include "IPBossCharacter.generated.h"

class AIPEnemyCharacter;
/**
 * 
 */
class AIPAIController;
class UBehaviorTree;
class UEnemyCombatComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInitNameDelegate, FName, BossName);


UCLASS()
class INVENTORYPROJECT_API AIPBossCharacter : public AIPCharacterBase, public IEnemyInterface, public IBossInterface
{
	GENERATED_BODY()

public:
	AIPBossCharacter();

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	virtual EBossClass GetBossClass_Implementation() override;
	virtual void PossessedBy(AController* NewController) override;

	/** Combat Interface **/
	virtual void Die(const FVector& DeathImpulse) override;
	virtual UPawnCombatComponent* GetPawnCombatComponent_Implementation() override;
	/** End Combat Interface **/

	/** Enemy Interface **/
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual UAnimMontage* GetDeathMontage_Implementation() override;
	virtual FProjectileInfo GetProjectileInfo_Implementation() const override;
	/** End Enemy Interface */

	/** Boss Interface **/
	virtual TArray<UAnimMontage*> GetMeleeAttackMontages_Implementation() override;
	virtual TArray<UAnimMontage*> GetRangeAttackMontages_Implementation() override;
	virtual UAnimMontage* GetSummonMontage_Implementation() override;
	virtual FSummonEnemyInfo GetSummonEnemyInfo_Implementation() override;
	/** End Boss Interface **/
	
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	AActor* CombatTarget;
	
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	void InitializeDefaultAttributesForBoss() const;
	virtual void OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(BlueprintAssignable)
	FOnInitNameDelegate BossNameDelegate;
	
private:
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;
	
	UPROPERTY(EditDefaultsOnly)
	EBossClass BossClass;
	
	UPROPERTY(EditAnywhere, Category = "Montage")
	TArray<UAnimMontage*> MeleeAttackMontages;
	
	UPROPERTY(EditAnywhere, Category = "Montage")
	TArray<UAnimMontage*> RangeAttackMontages;

	UPROPERTY(EditAnywhere, Category = "Montage")
	UAnimMontage* SummonEnemiesMontage;

	UPROPERTY(EditAnywhere, Category = "AI")
	AIPAIController * AIController;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UEnemyCombatComponent* BossCombatComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FProjectileInfo ProjectileInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName BossName;
	
	UPROPERTY()
	UIPUserWidget* BossHealthBarWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UIPUserWidget> BossHealthBarWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	FSummonEnemyInfo SummonEnemyInfo;
	
};
