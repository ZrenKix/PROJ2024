// © 2024 Choklad4Life. All rights reserved.


#include "Arena/Unit/ErebusBossUnit.h"

#include "DBGameplayTags.h"
#include "AbilitySystem/DBAbilitySystemLibrary.h"
#include "AbilitySystem/ProjAttributeSet.h"
#include "Arena/ArenaManager.h"
#include "Arena/Unit/HeroUnit.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PROJ/PROJ.h"
#include "UI/Widget/DBUserWidget.h"

AErebusBossUnit::AErebusBossUnit() : CurrentRound(1)
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UProjAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // Kolla https://imgur.com/a/qExGmrP för mer info
	
	AttributeSet = CreateDefaultSubobject<UProjAttributeSet>(TEXT("AttributeSet"));

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(GetRootComponent());

	TargetDecal = CreateDefaultSubobject<UDecalComponent>("TargetDecal");
	TargetDecal->SetupAttachment(GetRootComponent());
	TargetDecal->SetVisibility(false);
	
}

void AErebusBossUnit::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	AddDefaultAbilities();
	UDBAbilitySystemLibrary::GiveStartupAbilites(this, AbilitySystemComponent);
	Settings = GetWorld()->GetGameInstance()->GetSubsystem<UEnemySettings>();
	if (Settings)
	{
		MaxMana = Settings->GetErebusMaxMana();
		ChargeManaGain = Settings->GetErebusChargeManaGain();
		PhaseOneRndHeroChance = Settings->GetErebusPhaseOneRndHeroChance();
		PhaseTwoRndHeroChance = Settings->GetErebusPhaseTwoRndHeroChance();
		UE_LOG(LogTemp, Warning, TEXT("ErebusBoss: Initial MaxMana = %d"), MaxMana);
		UE_LOG(LogTemp, Warning, TEXT("ErebusBoss: Initial ChargeManaGain = %d"), ChargeManaGain);
		UE_LOG(LogTemp, Warning, TEXT("ErebusBoss: Initial PhaseOneRndHeroChance = %d"), PhaseOneRndHeroChance);
		UE_LOG(LogTemp, Warning, TEXT("ErebusBoss: Initial PhaseTwoRndHeroChance = %d"), PhaseTwoRndHeroChance);
		Settings->OnEnemySettingChanged.AddDynamic(this, &AErebusBossUnit::OnSettingsChanged);
	}
	MaxMana = 4;
	CurrentMana = 0;
	if (!ArenaManager)
	{
		ArenaManager = GetWorld()->GetSubsystem<UArenaManager>();
	}
	if(UDBUserWidget* DBUserWidget = Cast<UDBUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		DBUserWidget->SetWidgetController(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No widget found"));
	}

	if(const UProjAttributeSet* AS = CastChecked<UProjAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
				[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddLambda(
				[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		
		AbilitySystemComponent->RegisterGameplayTagEvent(FDBGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AErebusBossUnit::HitReactTagChanged
		);
		
		OnHealthChanged.Broadcast(AS->GetHealth());
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	}
}

void AErebusBossUnit::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(BeforeAttackDelay);
	}

	OnAbilityInputExecuted.Clear();
	OnHealthChanged.Clear();
	OnMaxHealthChanged.Clear();
}

void AErebusBossUnit::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
}

void AErebusBossUnit::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UProjAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	InitializeDefaultAttributes();
}

void AErebusBossUnit::OnTargeted_Implementation()
{
	if(TargetDecal)
	{
		TargetDecal->SetVisibility(true);
		UE_LOG(LogTemp, Display, TEXT("OnTargeted"));
	}
}

void AErebusBossUnit::OnTargetedEnd_Implementation()
{
	if(TargetDecal)
	{
		UE_LOG(LogTemp, Display, TEXT("OnTargetedEnd"));
		TargetDecal->SetVisibility(false);
	}
}

int32 AErebusBossUnit::GetPlayerLevel()
{
	return Level;
}


void AErebusBossUnit::Die()
{
	ArenaManager->EnemyUnits.Remove(this);
	UE_LOG(LogTemp, Display, TEXT("Dies"));
	PlayDeathAnimation();
	Super::Die();
}

void AErebusBossUnit::NotifyPlayerOfDeath()
{
	// Display a message to the player
	if (GEngine)
	{
		FString DeathMessage = FString::Printf(TEXT("%s has been defeated!"), *GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, DeathMessage);
	}

	// Optionally, you can implement more sophisticated UI feedback here
}

bool AErebusBossUnit::ActionTurn()
{
	UE_LOG(LogTemp, Log, TEXT("%s is preparing to attack..."), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("Current round: %d"), CurrentRound);
	UE_LOG(LogTemp, Warning, TEXT("Current mana: %d"), CurrentMana);

	
	Actions.Empty();
	if (bOnCooldown)
	{
		Actions.Add(FBossAction(1, EBossActionType::CoolDown));
	}

	if (CurrentRound >= nextRoundThreshold)
	{
		Actions.Add(FBossAction(2, EBossActionType::SpawnMinions));
	}

	if (CurrentMana == MaxMana)
	{
		Actions.Add(FBossAction(3, EBossActionType::AoEAttack));
	}

	if (CurrentMana < MaxMana)
	{
		Actions.Add(FBossAction(4, EBossActionType::Charge));
		Actions.Add(FBossAction(4, EBossActionType::BasicAttack));
	}
	
	Actions.Sort([](const FBossAction& A, const FBossAction& B)
	{
		if (A.Priority == B.Priority)
		{
			return FMath::RandBool();
		}
		return A.Priority < B.Priority;
	});
	GetWorld()->GetTimerManager().SetTimer(BeforeAttackDelay, this, &AErebusBossUnit::ExecuteNextAction, 1.0f, false);

	CurrentRound++;
	return true;
}

void AErebusBossUnit::ExecuteNextAction()
{
	if (Actions.Num() > 0)
	{
		AHeroUnit* Teddy = bIsTaunted ? FindHeroByName(TEXT("Teddy")) : nullptr;
		FBossAction SelectedAction = Actions[0];
		UE_LOG(LogTemp, Log, TEXT("Boss executes action: Priority %d"), SelectedAction.Priority);

		switch (SelectedAction.ActionType)
		{
		case EBossActionType::CoolDown:
			CoolDown();
			bOnCooldown = false;
			break;

		case EBossActionType::SpawnMinions:
			UE_LOG(LogTemp, Log, TEXT("Erebus spawns minions."));
			if (ArenaManager)
			{
				int32 IncreaseRounds = 3;
				if (Settings)
				{
					IncreaseRounds = Settings->GetErebusFailedMinionSpawnRoundIncrease();
				}
				bool bSpawnedMinions = ArenaManager->SpawnEnemies(ArenaManager->PendingEnemyClasses, 0, 2);
				if (!bSpawnedMinions)
				{
					UE_LOG(LogTemp, Log, TEXT("Could not spawn minions :("));
					Actions.RemoveAt(0);
					ExecuteNextAction();
				}
				nextRoundThreshold += bSpawnedMinions ? IncreaseRounds : IncreaseRounds;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Cannot spawn minions: ArenaManager is null"));
			}
			
			break;

		case EBossActionType::AoEAttack:
			AoeAttack();
			CurrentMana = 0;
			bOnCooldown = true;
			break;

		case EBossActionType::Charge:
			Charge();
			(Settings) ? CurrentMana += Settings->GetErebusChargeManaGain() : CurrentMana += ChargeManaGain;
			//CurrentMana += 2;
			break;

		case EBossActionType::BasicAttack:
			TargetHero = Teddy ? Teddy : SelectTarget();
			if (TargetHero) { BasicAttack(); }
			break; 

		default:
			UE_LOG(LogTemp, Warning, TEXT("Unknown action type"));
			break;
		}

		OnAbilityInputExecuted.Broadcast(this);
	}
}

void AErebusBossUnit::OnSettingsChanged(FName SettingName, float NewValue)
{
	if (SettingName == "Erebus_MaxMana")
	{
		MaxMana = (int)NewValue;
		UE_LOG(LogTemp, Warning, TEXT("ErebusBoss: MaxMana updated to %d"), MaxMana);
	}
	else if (SettingName == "Erebus_ChargeManaGain")
	{
		ChargeManaGain = (int)NewValue;
		UE_LOG(LogTemp, Warning, TEXT("ErebusBoss: ChargeManaGain updated to %d"), ChargeManaGain);
	}
	else if (SettingName == "Erebus_FailedMinionSpawnRoundIncrease")
	{
		nextRoundThreshold = (int)NewValue;
		UE_LOG(LogTemp, Warning, TEXT("ErebusBoss: FailedMinionSpawnRoundIncrease updated to %d"), nextRoundThreshold);
	}
	else if (SettingName == "Erebus_PhaseOneRndHeroChance")
	{
		PhaseOneRndHeroChance = NewValue;
		UE_LOG(LogTemp, Warning, TEXT("ErebusBoss: PhaseOneRndHeroChance updated to %d"), PhaseOneRndHeroChance);
	}
	else if (SettingName == "Erebus_PhaseTwoRndHeroChance")
	{
		PhaseTwoRndHeroChance = NewValue;
		UE_LOG(LogTemp, Warning, TEXT("ErebusBoss: PhaseTwoRndHeroChance updated to %d"), PhaseTwoRndHeroChance);
	}
}

AHeroUnit* AErebusBossUnit::FindHeroByName(const FString& Name) const
{
	for (AHeroUnit* Hero : ArenaManager->GetHeroes())
	{
		if (Hero && Hero->UnitName.Equals(Name))
		{
			return Hero;
		}
	}
	return nullptr;
}

AHeroUnit* AErebusBossUnit::SelectTarget()
{
	UE_LOG(LogTemp, Error, TEXT("Select target"));
	int32 RandomChance = FMath::RandRange(1, 100);
	(RandomChance <= GetRandomChanceByRound(CurrentRound)) ? TargetHero
		= SelectHeroWithLeastHealth() : TargetHero = RandomlySelectTarget();
	return TargetHero;
}

AHeroUnit* AErebusBossUnit::RandomlySelectTarget()
{
	if (ArenaManager->HeroUnits.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Herolist is 0"));
		return nullptr;
	}

	int32 RandomIndex = FMath::RandRange(0, ArenaManager->HeroUnits.Num() - 1);
	UE_LOG(LogTemp, Log, TEXT("Randomly selected hero: %s."), *ArenaManager->HeroUnits[RandomIndex]->GetName());
	return ArenaManager->HeroUnits[RandomIndex];
}

AHeroUnit* AErebusBossUnit::SelectHeroWithLeastHealth()
{
	if (ArenaManager->HeroUnits.Num() == 0)
		return nullptr;
	
	AHeroUnit* LowestHealthHero = ArenaManager->HeroUnits[0];
	for (AHeroUnit* Hero : ArenaManager->HeroUnits)
		if (Hero->CurrentHealth < LowestHealthHero->CurrentHealth)
			LowestHealthHero = Hero;
	UE_LOG(LogTemp, Log, TEXT("Selected hero with lowest health: %s."), *LowestHealthHero->GetName());
	return LowestHealthHero;
}

int32 AErebusBossUnit::GetRandomChanceByRound(int32 CurrentRounds) const
{
	return (CurrentRounds <= 6) ? PhaseOneRndHeroChance : PhaseTwoRndHeroChance;
}
