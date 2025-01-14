// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Arena/Unit/UnitPaperBase.h"
#include "Components/WidgetComponent.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "SmallShadowUnit.generated.h"

class UEnemySettings;
class UArenaManager;
class UWidgetComponent;

/**
 * 
 */
UCLASS()
class PROJ_API ASmallShadowUnit : public AUnitPaperBase
{
	GENERATED_BODY()
	ASmallShadowUnit();

public:
	// Health property
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float Health;

	// Function to determine if the enemy is alive
	UFUNCTION(BlueprintCallable, Category="Stats")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category="Action")
	void FinishedTurn();

	void NotifyPlayerOfDeath();

	UFUNCTION(BlueprintCallable)
	virtual bool ActionTurn() override;
	UFUNCTION(BlueprintImplementableEvent, Category="Combat")
	void Attack();

	UFUNCTION(BlueprintCallable)
	AHeroUnit* SelectTarget();
	UFUNCTION(BlueprintCallable)
	AHeroUnit* RandomlySelectTarget();
	UFUNCTION(BlueprintCallable)
	AHeroUnit* SelectHeroWithLeastHealth();
	UFUNCTION(BlueprintCallable)
	int32 GetRandomChanceByRound(int32 CurrentRound) const;

	UFUNCTION(BlueprintImplementableEvent, Category="Stats")
	void EndTauntEffect();

	UPROPERTY(BlueprintReadOnly, Category="Combats")
	AHeroUnit* TargetHero;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 250.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float LifeSpan = 1.f;

	UFUNCTION(BlueprintImplementableEvent, Category="Combat")
	void PlayDeathAnimation();
	
	/** ITargetInterface */
	// Override interface functions
	virtual void OnTargeted_Implementation() override;
	virtual void OnTargetedEnd_Implementation() override;
	/** End ITargetInterface */
	
	/** ICombatInterface */
	virtual int32 GetPlayerLevel() override;
	virtual void Die() override;
	/** End ICombatInterface */

	AHeroUnit* FindHeroByName(const FString& Name) const;
	
	
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	UFUNCTION()
	void OnSettingsChanged(FName SettingName, float NewValue);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;

	UPROPERTY()
	bool bIsFirstAttack = true;

	UPROPERTY()
	UEnemySettings* Settings;

	UPROPERTY()
	float Chance = 50.f;

	UPROPERTY()
	int32 CurrentRound = 0; 

	UPROPERTY()
	FTimerHandle BeforeAttackDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(VisibleAnywhere)
	UArenaManager* ArenaManager;
	
	TArray<AHeroUnit*> HeroList;	// List containing heroes from ArenaManager so the enemy can randomly select one from it.

};
