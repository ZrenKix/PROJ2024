// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "Arena/DialogueCondition.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Player/Controllers/ArenaPlayerController.h"
#include "UnitBase.generated.h"

class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;
class UGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionTurnDelegate);

UCLASS()
class PROJ_API AUnitBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AUnitBase();

	virtual void Die() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();
	virtual AActor* GetAvatar_Implementation() override;
	virtual bool IsDead_Implementation() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxHealth = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentHealth = MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDialogueCondition DialogueEntry;

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	
	void AddDefaultAbilities();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category="Combat")
	FVector PlayerLocation;
	
	UPROPERTY(BlueprintReadOnly)
	bool bDead = false;
	
	virtual FVector GetPlayerLocation() override;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level = 1.f) const;
	virtual void InitializeDefaultAttributes() const;


public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	virtual bool ActionTurn() PURE_VIRTUAL(AUnitBase::ActionTurn, return false;);
	virtual void OnDeath() PURE_VIRTUAL(AUnitBase::ActionTurn, );
	
	virtual int GainXp(int Amount);
	virtual bool LevelUp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Unit Image")
	UTexture2D* Image;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnAbilityInputExecuted OnAbilityInputExecuted;

private:

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
	
};
