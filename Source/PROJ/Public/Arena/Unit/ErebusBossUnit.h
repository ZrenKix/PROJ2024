// © 2024 Choklad4Life. All rights reserved.

#pragma once


#include "CoreMinimal.h"
#include "EnemySettings.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Arena/Unit/UnitPaperBase.h"
#include "Components/WidgetComponent.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "ErebusBossUnit.generated.h"


class UArenaManager;

UENUM(BlueprintType)
enum class EBossActionType : uint8
{
	CoolDown      UMETA(DisplayName = "Cooldown"),
	SpawnMinions  UMETA(DisplayName = "Spawn Minions"),
	AoEAttack     UMETA(DisplayName = "AoE Attack"),
	Charge        UMETA(DisplayName = "Charge"),
	BasicAttack   UMETA(DisplayName = "Basic Attack")
};

USTRUCT(BlueprintType)
struct FBossAction
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Priority;

	UPROPERTY()
	EBossActionType ActionType;

	FBossAction()
		: Priority(0), ActionType(EBossActionType::BasicAttack)
	{}

	FBossAction(int32 InPriority, EBossActionType InActionType)
		: Priority(InPriority), ActionType(InActionType)
	{}
};

UCLASS()
class PROJ_API AErebusBossUnit : public AUnitPaperBase
{
	
	GENERATED_BODY()
	AErebusBossUnit();
	public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	int nextRoundThreshold = 2;

	UPROPERTY()
	int ChargeManaGain = 2;
	
	void NotifyPlayerOfDeath();
	
	UFUNCTION(BlueprintCallable)
	virtual bool ActionTurn() override;
	UFUNCTION(BlueprintCallable)
	AHeroUnit* SelectTarget();
	UFUNCTION(BlueprintCallable)
	AHeroUnit* RandomlySelectTarget();
	UFUNCTION(BlueprintCallable)
	AHeroUnit* SelectHeroWithLeastHealth();
	UFUNCTION(BlueprintCallable)
	int32 GetRandomChanceByRound(int32 CurrentRounds) const;
	UFUNCTION(BlueprintCallable)
	void ExecuteNextAction();
	UFUNCTION()
	void OnSettingsChanged(FName SettingName, float NewValue);
	UFUNCTION()
	AHeroUnit* FindHeroByName(const FString& Name) const;

	//Abilities
	UFUNCTION(BlueprintImplementableEvent, Category="Abilities")
	void BasicAttack();
	UFUNCTION(BlueprintImplementableEvent, Category="Abilities")
	void AoeAttack();
	UFUNCTION(BlueprintImplementableEvent, Category="Abilities")
	void Charge();
	UFUNCTION(BlueprintImplementableEvent, Category="Abilities")
	void CoolDown();

	UPROPERTY(BlueprintReadOnly)
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
	UFUNCTION(BlueprintImplementableEvent, Category="Combat")
	void PlayDeathAnimation();
	virtual void Die() override;
	/** End ICombatInterface */
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitAbilityActorInfo() override;
	//virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;

	UPROPERTY()
	FTimerHandle BeforeAttackDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(VisibleAnywhere)
	UArenaManager* ArenaManager;

	UPROPERTY(VisibleAnywhere)
	TArray<FBossAction> Actions;

	UPROPERTY()
	bool bOnCooldown = false;

	UPROPERTY()
	int CurrentRound;

	UPROPERTY()
	UEnemySettings* Settings; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PhaseOneRndHeroChance = 50;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PhaseTwoRndHeroChance = 10;
};
