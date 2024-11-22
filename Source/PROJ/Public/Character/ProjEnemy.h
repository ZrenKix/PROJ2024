// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arena/Unit/UnitBase.h"
#include "Interaction/TargetInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "ProjEnemy.generated.h"

class UArenaManager;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class PROJ_API AProjEnemy : public AUnitBase, public ITargetInterface
{
	GENERATED_BODY()
	AProjEnemy();

public:

	/** TILLFÄLLIGT */

	// Health property
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float Health;

	// Function to determine if the enemy is alive
	UFUNCTION(BlueprintCallable, Category="Stats")
	bool IsAlive() const;

	void Attack(class ABaseCharacter* Target);
	void NotifyPlayerOfDeath();

	UFUNCTION(BlueprintCallable)
	virtual bool ActionTurn() override;

	AHeroUnit* RandomlySelectTarget();

	AHeroUnit* TargetHero;

	/** TILLFÄLLIGT SLUT */

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 250.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float LifeSpan = 5.f;

	
	/** ITargetInterface */
	// Override interface functions
	virtual void OnTargeted_Implementation() override;
	virtual void OnTargetedEnd_Implementation() override;
	/** End ITargetInterface */
	
	/** ICombatInterface */
	virtual int32 GetPlayerLevel() override;
	virtual void Die() override;
	/** End ICombatInterface */

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(VisibleAnywhere)
	UArenaManager* ArenaManager;
	
	TArray<AHeroUnit*> HeroList;	// List containing heroes from ArenaManager so the enemy can randomly select one from it.

};
