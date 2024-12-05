// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "Arena/DialogueCondition.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Player/Controllers/ArenaPlayerController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Unit")
	FString UnitName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Unit")
	int MaxHealth = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Unit")
	int CurrentHealth = MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Unit")
	int MaxMana = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Unit")
	int CurrentMana = MaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Unit")
	FDialogueCondition DialogueEntry;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnActionTurnDelegate OnActionTurn;


	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

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

	UPROPERTY(BlueprintReadWrite, Category="Combat")
	bool bIsTaunted= false;
	
	virtual FVector GetPlayerLocation() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level = 1.f) const;
	virtual void InitializeDefaultAttributes() const;


public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	virtual bool ActionTurn() PURE_VIRTUAL(AUnitBase::ActionTurn, return false;);
	virtual void OnDeath() PURE_VIRTUAL(AUnitBase::ActionTurn, );
	
	virtual int GainXp(int Amount);
	virtual bool LevelUp();

	UFUNCTION(BlueprintCallable, Category="Combat")
	bool EnoughManaFor(int value) const;

	UFUNCTION(BlueprintCallable, Category="Unit Attributes")
	void ChangeMana(int value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Card Image")
	UTexture2D* CardImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Queue Image")
    UTexture2D* QueueImage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Queue Selected Image")
    UTexture2D* QueueSelectedImage;
    	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Border Image")
    UTexture2D* BorderImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Border Selected Image")
    UTexture2D* BorderSelectedImage;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnAbilityInputExecuted OnAbilityInputExecuted;

private:

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
	
};
