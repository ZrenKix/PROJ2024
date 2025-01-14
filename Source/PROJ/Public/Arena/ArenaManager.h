// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Arena/Unit/UnitBase.h"
#include "Player/Controllers/ArenaPlayerController.h"
#include "ArenaManager.generated.h"

class ADreamboundGameMode;
class UArenaDialogueLogic;
class AFighterAI;
class AHeroUnit;

UENUM(BlueprintType)
enum class EBossfightType : uint8
{
	None	UMETA(DisplayName = "No Boss"),
	Erebus	UMETA(DisplayName = "Erebus Boss"),
	DarkBob UMETA(DisplayName = "Dark Bob")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatFinished, bool, isVictory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnErebusDied);

UCLASS(Blueprintable)
class PROJ_API UArenaManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// Constructor
	UArenaManager();

	// Override Initialize and Deinitialize
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Arena")
	bool CreateArena(TArray<TSubclassOf<AUnitBase>> EnemyClasses, TArray<TSubclassOf<AUnitBase>> HeroClasses, FName LevelName, EBossfightType InBossType, int numHeroes);

	UFUNCTION(BlueprintCallable, Category = "Arena Spawning")
	bool SpawnEnemies(const TArray<TSubclassOf<AUnitBase>>& EnemyClasses, int32 StartIndex = 0, int32 EndIndex = 2);

	UFUNCTION(BlueprintCallable, Category = "Arena Spawning")
	bool SpawnHeroes(const TArray<TSubclassOf<AUnitBase>>& HeroClasses, int32 StartIndex = 0, int32 EndIndex = 2);

	UFUNCTION()
	bool IsPositionOccupied(const FVector& Position) const;
	
	UFUNCTION(BlueprintCallable)
	bool SetTurnOrder();

	UFUNCTION(BlueprintCallable)
	void NextInTurnOrder();

	UFUNCTION(BlueprintCallable)
	bool CheckState();

	UFUNCTION()
	void RemoveUnit(AUnitBase* UnitBase);

	// Expose the delegate for Blueprint assignment
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCombatFinished OnCombatFinished;

	// Expose the delegate for Blueprint assignment
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnActionStart OnActionStart;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnActionEnd OnActionEndEvent;
	
	void FinishCombat(bool isVictory);
	
	bool CancelCombat();

	UFUNCTION(BlueprintCallable, Category = "Arena")
	TArray<AHeroUnit*> GetHeroes() const
	{
		return HeroUnits;
	}

	//Getters
	UFUNCTION(BlueprintCallable, Category = "Arena Units")
	TArray<AUnitBase*> GetUnitQueue() const;

	UFUNCTION(BlueprintCallable, Category = "Arena Rounds")
	int32 GetCurrentRound() const { return CurrentRound; }
	
	// Callback function for when the level finishes loading
	UFUNCTION()
	void OnLevelLoaded();
	void SpawnAndPlaceUnits();

	void OnLevelUnload(ULevel* Level, UWorld* World);
	void Cleanup();

	FTimerHandle EndTurnTimerHandle; // Timer handle for delayed check
	bool bIsTurnInProgress;

	UFUNCTION(BlueprintCallable, Category = "Arena Units")
	void OnActionEnd(AUnitBase* Unit); // On end of turn
	void CheckStateDelay();
	void StartNewRound();

	UPROPERTY()
	AArenaPlayerController* HeroController;
	
	UPROPERTY(BlueprintReadOnly, Category="Arena")
	EBossfightType CurrentBossfightType = EBossfightType::None;
	
	UPROPERTY()
	int StoredXp;

	UPROPERTY(BlueprintReadOnly, Category="Arena")
	bool bHasErebusBossUnit;

	UPROPERTY(BlueprintReadOnly, Category="Arena")
	int HeroesToSpawn;
	
	UPROPERTY()
	bool isDoubleAction = false;
	
	UPROPERTY()
	int CurrentRound = 1;

	UPROPERTY()
	int UnitsActedThisRound = 0;
	
	UPROPERTY()
	int TotUnitsThisRound = 0;
	
	UPROPERTY()
	bool bIsBossfight = false;
	
	// Units in combat
	UPROPERTY()
	TArray<AHeroUnit*> HeroUnits;

	UPROPERTY(BlueprintReadOnly, Category="Arena")
	TArray<AUnitBase*> EnemyUnits;

	UPROPERTY()
	// Hardcoded coordinates for placement
	TArray<FVector> PlacementCoordinates;

	UPROPERTY()
	// Hardcoded coordinates for placement
	TArray<FVector> PlacementCoordinatesHeroes;
	// Turn order
	UPROPERTY()
	TArray<AUnitBase*> UnitPriorityQueue;

	UPROPERTY()
	TArray<TSubclassOf<AUnitBase>> PendingEnemyClasses;

	UPROPERTY()
	TArray<TSubclassOf<AUnitBase>> PendingHeroClasses;

	UPROPERTY()
	TArray<AUnitBase*> SpawnedEnemies; 

	// Member variable to store the level name
	FName LoadedLevelName;

	UPROPERTY()
	APawn* OriginalPawn;

	// Member variable to store the LevelStreaming reference
	UPROPERTY()
	ULevelStreaming* LevelStreaming;
	
	UPROPERTY(BlueprintReadOnly, Category="Dialogue System")
	UArenaDialogueLogic* DialogueLogic;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnErebusDied OnErebusDied;
};
