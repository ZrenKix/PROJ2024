// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Arena/Unit/UnitPaperBase.h"
#include "Components/WidgetComponent.h"
#include "DarkBobUnit.generated.h"

class UArenaManager;

UENUM(BlueprintType)
enum class EBossfightType : uint8
{
	None UMETA(DisplayName = "No Boss"),
	Erebus UMETA(DisplayName = "Erebus Boss"),
	NewBoss UMETA(DisplayName = "New Boss")
};

UENUM(BlueprintType)
enum class EBossActionTypeDB : uint8
{
	CoolDown      UMETA(DisplayName = "Cooldown"),
	SpawnMinions  UMETA(DisplayName = "Spawn Minions"),
	AoEAttack     UMETA(DisplayName = "AoE Attack"),
	Charge        UMETA(DisplayName = "Charge"),
	BasicAttack   UMETA(DisplayName = "Basic Attack")
};

USTRUCT(BlueprintType)
struct FBossActionDB
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Priority;

	UPROPERTY()
	EBossActionTypeDB ActionType;

	FBossActionDB()
		: Priority(0), ActionType(EBossActionTypeDB::BasicAttack)
	{}

	FBossActionDB(int32 InPriority, EBossActionTypeDB InActionType)
		: Priority(InPriority), ActionType(InActionType)
	{}
};

UCLASS()
class PROJ_API ADarkBobUnit : public AUnitPaperBase
{
	
	GENERATED_BODY()
	ADarkBobUnit();
	public:

	/** TILLFÄLLIGT */

	// Health property
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	int nextRoundThreshold = 10; 
	
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
	
	virtual void OnTargeted_Implementation() override;
	virtual void OnTargetedEnd_Implementation() override;
	
	virtual int32 GetPlayerLevel() override;
	virtual void Die() override;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitAbilityActorInfo() override;

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
	TArray<FBossActionDB> Actions;
	
	TArray<AHeroUnit*> HeroList;

	UPROPERTY()
	bool bOnCooldown = false;

	UPROPERTY()
	int CurrentRound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PhaseOneRndHeroChance = 50;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PhaseTwoRndHeroChance = 10;
};

