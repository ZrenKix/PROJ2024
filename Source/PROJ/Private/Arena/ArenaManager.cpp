// © 2024 Choklad4Life. All rights reserved.

#include "Arena/ArenaManager.h"

#include "DreamboundGameMode.h"
#include "EnemySettings.h"
#include "Arena/Unit/HeroUnit.h"
#include "TimerManager.h"
#include "Arena/ArenaDialogueLogic.h"
#include "Arena/Unit/DarkBobUnit.h"
#include "Arena/Unit/ErebusBossUnit.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/LevelStreaming.h"
#include "Engine/World.h"


UArenaManager::UArenaManager()
: bIsTurnInProgress(false), StoredXp(0),
PlacementCoordinates({
	FVector(7641.637582,7060.630225,4916.368207), // Top
	FVector(7894.296125,6698.979576,4916.368207), // Middle
	FVector(8012.726447,6950.826909,4916.368207), // Bottom
	FVector(7286.061449,6894.000128,4852.012842) // Boss
}),
PlacementCoordinatesHeroes({
	FVector(7639.792119,7554.095495,4928.314536), // Edith
	FVector(7756.814990,7780.386806,4928.314508), // Teddy
	FVector(7358.323440,7723.813914,4928.314516) // Vincent
})
{
}

void UArenaManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	bIsTurnInProgress = false;
	StoredXp = 0;

	// Register to level unload delegate
	FWorldDelegates::LevelRemovedFromWorld.AddUObject(this, &UArenaManager::OnLevelUnload);
}

void UArenaManager::Deinitialize()
{
	// Unregister delegates
	FWorldDelegates::LevelRemovedFromWorld.RemoveAll(this);

	Cleanup();

	Super::Deinitialize();
}

void UArenaManager::OnLevelUnload(ULevel* Level, UWorld* World)
{
	if (Level && World == GetWorld())
	{
		Cleanup();

		bIsTurnInProgress = false;
		
		// Remove the level from the LoadedLevelName variable
		LoadedLevelName = NAME_None;
	}
}

void UArenaManager::Cleanup()
{
	for (AUnitBase* Unit : GetUnitQueue())
	{
		if (Unit)
		{
			Unit->OnAbilityInputExecuted.RemoveDynamic(this, &UArenaManager::OnActionEnd);
			Unit->Destroy();
		}
	}
	for (AUnitBase* Hero : HeroUnits)
	{
		if (Hero)
		{
			Hero->OnAbilityInputExecuted.RemoveDynamic(this, &UArenaManager::OnActionEnd);
			Hero->Destroy();
		}
	}
	EnemyUnits.Empty();
	SpawnedEnemies.Empty();
	HeroUnits.Empty();
	UnitsActedThisRound = 0;
	TotUnitsThisRound = 0;
	HeroController = nullptr;
	StoredXp = 0;
	UnitPriorityQueue.Empty();
	bIsTurnInProgress = false;

	DialogueLogic = nullptr;
	
	if (LevelStreaming)
	{
		LevelStreaming->OnLevelLoaded.RemoveDynamic(this, &UArenaManager::OnLevelLoaded);
	}
}

bool UArenaManager::CreateArena(TArray<TSubclassOf<AUnitBase>> EnemyClasses, TArray<TSubclassOf<AUnitBase>> HeroClasses,FName LevelName,  EBossfightType InBossType, int NumHeroes)
{	
	// Ensure GetWorld() is valid
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("WorldContext is null"));
		return false;
	}

	// Store the enemy classes for use after the level is loaded
	PendingEnemyClasses = EnemyClasses;
	PendingHeroClasses = HeroClasses;
	
	CurrentBossfightType = InBossType;
	HeroesToSpawn = NumHeroes;
	// Store original pawn
	OriginalPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	// Store the level name for later use
	LoadedLevelName = LevelName;

	// Retrieve the GameMode
	ADreamboundGameMode* DreamboundGameMode = Cast<ADreamboundGameMode>(GetWorld()->GetAuthGameMode());
	if (!DreamboundGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve GameMode"));
		return false;
	}
	
	AArenaPlayerController* PlayerController = Cast<AArenaPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve PlayerController"));
		return false;
	}

	if (!HeroController)
	{
		HeroController = PlayerController;
		HeroController->OnAbilityInputExecuted.AddDynamic(this, &UArenaManager::OnActionEnd);
	}
	
	DialogueLogic = NewObject<UArenaDialogueLogic>();
	if (DialogueLogic)
	{
		DialogueLogic->ArenaManager = this;
	}
	
	// Get the streaming level using the specified LevelName
	LevelStreaming = UGameplayStatics::GetStreamingLevel(GetWorld(), LevelName);
	if (!LevelStreaming)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find streaming level '%s'"), *LevelName.ToString());
		return false;
	}

	// Bind to the OnLevelLoaded delegate
	LevelStreaming->OnLevelLoaded.AddDynamic(this, &UArenaManager::OnLevelLoaded);

	// Try to load level
	if (!LevelStreaming->IsLevelLoaded())
	{
		UE_LOG(LogTemp, Warning, TEXT("Loading level '%s'"), *LevelName.ToString());
		LevelStreaming->SetShouldBeLoaded(true);
		LevelStreaming->SetShouldBeVisible(true);
	}
	else
	{
		// Level is already loaded
		OnLevelLoaded();
	}

	return true; // Indicate arena successfully was created
}

bool UArenaManager::SpawnEnemies(const TArray<TSubclassOf<AUnitBase>>& EnemyClasses, int32 StartIndex, int32 EndIndex)
{
	if (EnemyClasses.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No enemy classes to spawn."));
        return false;
    }

    // Check if all positions are available
    for (int32 PositionIndex = StartIndex; PositionIndex <= EndIndex; ++PositionIndex)
    {
        // Ensure the index is within the bounds of PlacementCoordinates
        if (!PlacementCoordinates.IsValidIndex(PositionIndex))
        {
            UE_LOG(LogTemp, Warning, TEXT("Position index %d is out of bounds."), PositionIndex);
            return false; // Cannot proceed if any position is invalid
        }

        FVector Position = PlacementCoordinates[PositionIndex];

        // Check if position is occupied
        if (IsPositionOccupied(Position))
        {
            UE_LOG(LogTemp, Warning, TEXT("Position %d at %s is occupied. Cannot spawn enemies."), PositionIndex, *Position.ToString());
            return false; // Positions are not all free
        }
    }

    // All positions are available; proceed to spawn enemies
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World context is null."));
        return false;
    }

    ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(World, LoadedLevelName);
    if (!StreamingLevel || !StreamingLevel->GetLoadedLevel())
    {
        UE_LOG(LogTemp, Error, TEXT("Streaming level '%s' not found or not loaded."), *LoadedLevelName.ToString());
        return false;
    } 

    ULevel* LoadedLevel = StreamingLevel->GetLoadedLevel();

    int32 EnemyIndex = 0;
    for (int32 PositionIndex = StartIndex; PositionIndex <= EndIndex && EnemyIndex < EnemyClasses.Num(); ++PositionIndex)
    {
        FVector SpawnLocation = PlacementCoordinates[PositionIndex];
        TSubclassOf<AUnitBase> EnemyClass = EnemyClasses[EnemyIndex];

        if (!EnemyClass)
        {
            UE_LOG(LogTemp, Error, TEXT("Enemy class at index %d is null"), EnemyIndex);
            ++EnemyIndex;
            continue;
        }

        FRotator SpawnRotation = FRotator::ZeroRotator;

        FActorSpawnParameters SpawnParams;
        SpawnParams.OverrideLevel = LoadedLevel;

        AUnitBase* SpawnedEnemy = World->SpawnActor<AUnitBase>(EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (!SpawnedEnemy)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor of class %s"), *EnemyClass->GetName());
            ++EnemyIndex;
            continue;
        }

        UE_LOG(LogTemp, Display, TEXT("Enemy unit '%s' spawned at location %s."), *SpawnedEnemy->GetName(), *SpawnLocation.ToString());

        // Ensure the actor is visible and has collision enabled
        SpawnedEnemy->SetActorHiddenInGame(false);
        SpawnedEnemy->SetActorEnableCollision(true);

        // Add to EnemyUnits array
        EnemyUnits.Add(SpawnedEnemy);
    	
    	if(!Cast<AErebusBossUnit>(SpawnedEnemy))
    	{
    		SpawnedEnemies.Add(SpawnedEnemy);
    	}
        // Bind input event
        SpawnedEnemy->OnAbilityInputExecuted.AddDynamic(this, &UArenaManager::OnActionEnd);
		
        ++EnemyIndex;
    }

	for (auto Enemy : EnemyUnits)
	{
		UE_LOG(LogTemp, Display, TEXT("Enemy unit in enemyunits list'%s'"), *Enemy->GetName());
	}
	if (!SpawnedEnemies.IsEmpty())
	{
		for (auto Enemy : SpawnedEnemies)
		{
			UE_LOG(LogTemp, Display, TEXT("Enemy unit in spawnedenemies list'%s'"), *Enemy->GetName());
		}
	}
    return true; 
}

bool UArenaManager::SpawnHeroes(const TArray<TSubclassOf<AUnitBase>>& HeroClasses, int32 StartIndex, int32 EndIndex)
{
	if (HeroClasses.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No hero classes to spawn."));
        return false;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World context is null."));
        return false;
    }

    ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(World, LoadedLevelName);
    if (!StreamingLevel || !StreamingLevel->GetLoadedLevel())
    {
        UE_LOG(LogTemp, Error, TEXT("Streaming level '%s' not found or not loaded."), *LoadedLevelName.ToString());
        return false;
    }

    ULevel* LoadedLevel = StreamingLevel->GetLoadedLevel();

    int32 HeroIndex = 0;
    for (int32 PositionIndex = StartIndex; PositionIndex <= EndIndex && HeroIndex < HeroClasses.Num(); ++PositionIndex)
    {
        // Kontrollera att indexet är inom räckvidden för PlacementCoordinatesHeroes
        if (!PlacementCoordinatesHeroes.IsValidIndex(PositionIndex))
        {
            UE_LOG(LogTemp, Warning, TEXT("Hero position index %d is out of bounds."), PositionIndex);
            return false;
        }

        FVector SpawnLocation = PlacementCoordinatesHeroes[PositionIndex];
        TSubclassOf<AUnitBase> HeroClass = HeroClasses[HeroIndex];

        if (!HeroClass)
        {
            UE_LOG(LogTemp, Error, TEXT("Hero class at index %d is null"), HeroIndex);
            ++HeroIndex;
            continue;
        }

        FRotator SpawnRotation = FRotator::ZeroRotator;

        FActorSpawnParameters SpawnParams;
        SpawnParams.OverrideLevel = LoadedLevel;

        AUnitBase* SpawnedHero = World->SpawnActor<AUnitBase>(HeroClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (!SpawnedHero)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor of class %s"), *HeroClass->GetName());
            ++HeroIndex;
            continue;
        }

        UE_LOG(LogTemp, Display, TEXT("Hero unit '%s' spawned at location %s."), *SpawnedHero->GetName(), *SpawnLocation.ToString());

        // Se till att aktören är synlig och har kollisionsdetektion aktiverad
        SpawnedHero->SetActorHiddenInGame(false);
        SpawnedHero->SetActorEnableCollision(true);

        // Lägg till i HeroUnits-arrayen
        AHeroUnit* HeroUnit = Cast<AHeroUnit>(SpawnedHero);
        if (HeroUnit)
        {
            HeroUnits.Add(HeroUnit);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Spawned actor %s is not an AHeroUnit"), *SpawnedHero->GetName());
        }
		
        // Koppla input-event
        SpawnedHero->OnAbilityInputExecuted.AddDynamic(this, &UArenaManager::OnActionEnd);

        ++HeroIndex;
    }
	UE_LOG(LogTemp, Display, TEXT("Final HeroUnits count: %d"), HeroUnits.Num());
    return true;
}

bool UArenaManager::IsPositionOccupied(const FVector& Position) const
{
	const float Tolerance = 5.0f; 
	
	for (const AUnitBase* Unit : EnemyUnits)
	{
		if (Unit && Unit->GetActorLocation().Equals(Position, Tolerance))
		{
			return true;
		}
	}
	return false;
}


TArray<AUnitBase*> UArenaManager::GetUnitQueue() const
{
	return UnitPriorityQueue;
}

void UArenaManager::OnLevelLoaded()
{
	if (!LevelStreaming)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelStreaming is null in OnLevelLoaded"));
		return;
	}
	
	ULevel* Level = LevelStreaming->GetLoadedLevel();
	if (!Level)
	{
		UE_LOG(LogTemp, Error, TEXT("Level '%s' is not loaded after OnLevelLoaded"), *LoadedLevelName.ToString());
		return;
	}

	// Unbind from the delegate to prevent multiple calls
	LevelStreaming->OnLevelLoaded.RemoveDynamic(this, &UArenaManager::OnLevelLoaded);

	UE_LOG(LogTemp, Display, TEXT("Level '%s' loaded successfully"), *LoadedLevelName.ToString());

	// Get the UWorld from the loaded level
	UWorld* LevelWorld = Level->GetWorld();
	if (!LevelWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get UWorld from Level"));
		return;
	}
	
	// Spawn enemies in the correct world
	SpawnAndPlaceUnits();
}

void UArenaManager::SpawnAndPlaceUnits()
{
	if (CurrentBossfightType == EBossfightType::Erebus)
	{
		SpawnEnemies(PendingEnemyClasses, 0, 3);
	}
	if (CurrentBossfightType == EBossfightType::DarkBob)
	{
		SpawnEnemies(PendingEnemyClasses, 0, 0);
	}
	bool bEnemiesSpawned = SpawnEnemies(PendingEnemyClasses, 0, 2);
	if (!bEnemiesSpawned)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemies could not be spawned because positions 0 to 2 are occupied."));
	}

	bool bHeroesSpawned = SpawnHeroes(PendingHeroClasses, 0, HeroesToSpawn);
	if (!bHeroesSpawned)
	{
		UE_LOG(LogTemp, Warning, TEXT("Heroes could not be spawned."));
	}
}

bool UArenaManager::SetTurnOrder()
{
	if (HeroUnits.IsEmpty() && EnemyUnits.IsEmpty()) return false; 

	for (AHeroUnit* Hero : HeroUnits)
	{
		if (Hero)
		{
			UnitPriorityQueue.Add(Hero);
		}
	}
	for (AUnitBase* Enemy : EnemyUnits)
	{
		if (Enemy)
		{
			UnitPriorityQueue.Add(Enemy);
		}
	}
	TotUnitsThisRound = HeroUnits.Num() + EnemyUnits.Num();
	UnitsActedThisRound = 0;
	return true;
}

bool UArenaManager::CheckState() // Returns true if combat is over or invalid, else false to continue
{
	if (!GetWorld())
		return false;

	bHasErebusBossUnit = false;
	if (CurrentBossfightType == EBossfightType::Erebus)
	{
		bHasErebusBossUnit = EnemyUnits.ContainsByPredicate([](AUnitBase* Unit)
		{
		return Unit && Unit->IsA<AErebusBossUnit>();
		});
	}
	
	
	if (EnemyUnits.IsEmpty() || (!bHasErebusBossUnit && CurrentBossfightType == EBossfightType::Erebus))
	{
		for (AUnitBase* Unit : EnemyUnits)
		{
			UE_LOG(LogTemp, Display, TEXT("%s has been reached"), *Unit->GetName());
		}
		if (CurrentBossfightType == EBossfightType::Erebus && !bHasErebusBossUnit)
		{
			UE_LOG(LogTemp, Display, TEXT("Erebus Boss not found - broadcasting OnErebusDied"));
			OnErebusDied.Broadcast();
		}
		UE_LOG(LogTemp, Error, TEXT("You WON!"));
		FinishCombat(true);
	}
	else if (HeroUnits.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("You LOSE!"));
		//OnCombatFinished.Broadcast();
		FinishCombat(false);

		//FName CurrentLevelName = *GetWorld()->GetMapName();
		//UGameplayStatics::OpenLevel(GetWorld(), CurrentLevelName, true);
	}
	else
	{
		return false; // Continue combat
	}
	
	return true; // Combat state invalid or finished
}

void UArenaManager::RemoveUnit(AUnitBase* Unit)
{
	UnitPriorityQueue.Remove(Unit);

	if (AHeroUnit* Hero = Cast<AHeroUnit>(Unit))
		HeroUnits.Remove(Hero);
	else
		EnemyUnits.Remove(Unit);
}

void UArenaManager::NextInTurnOrder()
{
	if (bIsTurnInProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("Turn logic is already in progress."));
		return;
	}

	if (UnitPriorityQueue.IsEmpty())
	{
		if (!SetTurnOrder())
		{
			UE_LOG(LogTemp, Error, TEXT("No units to queue in turn order"));
			bIsTurnInProgress = false;
			return;
		}
	}

	AUnitBase* Unit = UnitPriorityQueue[0];
	if (Unit)
	{
		bIsTurnInProgress = true;
		UE_LOG(LogTemp, Display, TEXT("Unit %s: Called OnActionTurn"), *Unit->GetName());

		if (AHeroUnit* Hero = Cast<AHeroUnit>(Unit))
		{
			UE_LOG(LogTemp, Display, TEXT("Hero's turn"));
			Hero->ActionTurn();
			HeroController->PossessHero(Hero);
		}
		else
		{
			if (!EnemyUnits.Contains(Unit))
			{
				UE_LOG(LogTemp, Error, TEXT("%s not in list, calling next in order"), *Unit->GetName());
				bIsTurnInProgress = false;
				UnitPriorityQueue.Pop();
				NextInTurnOrder();
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Enemy's turn"));
				Unit->ActionTurn();
			}
		}
		OnActionStart.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to peek unit from turn order"));
		bIsTurnInProgress = false;
		UnitPriorityQueue.Pop();
		NextInTurnOrder();
	}
}

void UArenaManager::OnActionEnd(AUnitBase* Unit)
{
	// Peek if correct unit is ending its turn
	AUnitBase* ExpectedUnit = UnitPriorityQueue[0];
	if (!ExpectedUnit)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get first unit in turn order"));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("Expected unit %s to turn order"), *ExpectedUnit->GetName()); // fortsätt här
	if (ExpectedUnit != Unit)
	{
		UE_LOG(LogTemp, Error, TEXT("Unit mismatch in turn order"));
		return;
	}

	// Dequeue the unit since it finished its turn and requeue to put it last
	UnitPriorityQueue.Remove(Unit);
	UnitPriorityQueue.Add(Unit);

	OnActionEndEvent.Broadcast();

	UnitsActedThisRound++;
	if (UnitsActedThisRound >= TotUnitsThisRound)
	{
		StartNewRound();
	}
	UE_LOG(LogTemp, Display, TEXT("Unit %s: Called OnActionTurn"), *Unit->GetName());
	
	GetWorld()->GetTimerManager().SetTimer(EndTurnTimerHandle, this, &UArenaManager::CheckStateDelay, 2, false);
}

void UArenaManager::CheckStateDelay()
{
	if (!CheckState())
	{
		UE_LOG(LogTemp, Display, TEXT("Continue combat"));
		bIsTurnInProgress = false;
		NextInTurnOrder();
	}
}

void UArenaManager::StartNewRound()
{
	if (!HeroUnits.IsEmpty())
	{
		for (AHeroUnit* Hero : HeroUnits)
		{
			if (Hero)
			{
				UE_LOG(LogTemp, Display, TEXT("Hero in HeroUnits during StartNewRound: %s"), *Hero->GetName());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HeroUnits is empty during StartNewRound."));
	}
	//hämtar enemy settings
	UEnemySettings* Settings = GetWorld()->GetGameInstance()->GetSubsystem<UEnemySettings>();
	
	TArray<AUnitBase*> Units =  GetUnitQueue();
	TArray<AUnitBase*> TempUnits;
	if (!Units.IsEmpty())
	{
		for (AUnitBase* Unit : Units)
		{
			if (ADarkBobUnit* DarkBob = Cast<ADarkBobUnit>(Unit))
			{
				//lägger till bob två gånger om villkoren uppfylls, defualt är 50% chans
				const float Chance = Settings ? Settings->GetDarkBobDoubleQueueChance() : 50.0f;
				UE_LOG(LogTemp, Warning, TEXT("DarkBob_DoubleQueueChance: %f"), Chance);
				isDoubleAction = (FMath::RandRange(0, 100) <= Chance);
				if (isDoubleAction) 
				{
					TempUnits.Add(DarkBob);
				}
			}
		}
		Units.Append(TempUnits);
		UnitPriorityQueue.Empty();
		for (AUnitBase* U : Units)
		{
			UnitPriorityQueue.Add(U);
		}

		TotUnitsThisRound = Units.Num();
		UnitsActedThisRound = 0;
	}
	
}

void UArenaManager::FinishCombat(bool isVictory)
{	
	OnCombatFinished.Broadcast(isVictory);

	bIsTurnInProgress = false;
	/*
	if (!LoadedLevelName.IsNone())
	{
		UGameplayStatics::UnloadStreamLevel(GetWorld(), LoadedLevelName, FLatentActionInfo(), true);
	}*/
	Cleanup();
}


bool UArenaManager::CancelCombat()
{
	return true;
}