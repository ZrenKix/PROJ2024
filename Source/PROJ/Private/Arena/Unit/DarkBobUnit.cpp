// © 2024 Choklad4Life. All rights reserved.


#include "Arena/Unit/DarkBobUnit.h"

#include "DBGameplayTags.h"
#include "EnemySettings.h"
#include "AbilitySystem/DBAbilitySystemLibrary.h"
#include "AbilitySystem/ProjAttributeSet.h"
#include "Arena/ArenaManager.h"
#include "Arena/Unit/HeroUnit.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PROJ/PROJ.h"
#include "UI/Widget/DBUserWidget.h"

ADarkBobUnit::ADarkBobUnit()
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

void ADarkBobUnit::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
	AddDefaultAbilities();
	UDBAbilitySystemLibrary::GiveStartupAbilites(this, AbilitySystemComponent);
	Settings = GetWorld()->GetGameInstance()->GetSubsystem<UEnemySettings>();
	if (Settings)
	{
		MaxMana = Settings->GetDarkBobMaxMana();
		ChargeManaGain = Settings->GetDarkBobChargeManaGain();
		Settings->OnEnemySettingChanged.AddDynamic(this, &ADarkBobUnit::OnSettingsChanged);
	}
	UE_LOG(LogTemp, Warning, TEXT("DarkBob: Initial MaxMana = %d"), MaxMana);
	UE_LOG(LogTemp, Warning, TEXT("DarkBob: Initial ChargeManaGain = %d"), ChargeManaGain);
	//MaxMana = 2;
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
			&ADarkBobUnit::HitReactTagChanged
		);
		
		OnHealthChanged.Broadcast(AS->GetHealth());
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	}
}

void ADarkBobUnit::NotifyPlayerOfDeath()
{
	// Display a message to the player
	if (GEngine)
	{
		FString DeathMessage = FString::Printf(TEXT("%s has been defeated!"), *GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, DeathMessage);
	}

	// Optionally, you can implement more sophisticated UI feedback here
}

bool ADarkBobUnit::ActionTurn()
{
	UE_LOG(LogTemp, Log, TEXT("%s is preparing to attack..."), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("Current round: %d"), CurrentRound);

	Actions.Empty();

	if (CurrentMana >= MaxMana && LastActionType != EBossActionTypeDB::DarkBobSpecial)
	{
		Actions.Add(FBossActionDB(1, EBossActionTypeDB::DarkBobSpecial));
	}
	else
	{
		if (LastActionType != EBossActionTypeDB::BasicAttack)
		{
			Actions.Add(FBossActionDB(2, EBossActionTypeDB::BasicAttack));
		}
		if(LastActionType != EBossActionTypeDB::Charge && CurrentMana < MaxMana)
		{
			Actions.Add(FBossActionDB(2, EBossActionTypeDB::Charge));
		}
	}
	if (Actions.Num() == 0){Actions.Add(FBossActionDB(5, EBossActionTypeDB::BasicAttack));}
	
	Actions.Sort([](const FBossActionDB& A, const FBossActionDB& B)
	{
		if (A.Priority == B.Priority)
		{
			return FMath::RandBool();
		}
		return A.Priority < B.Priority;
	});
	GetWorld()->GetTimerManager().SetTimer(BeforeAttackDelay, this, &ADarkBobUnit::ExecuteNextAction, 1.0f, false);
	if (!ArenaManager->isDoubleAction)
	{
		CurrentRound++;
	}
	return true;
}

AHeroUnit* ADarkBobUnit::SelectTarget()
{
	UE_LOG(LogTemp, Log, TEXT("HeroList size: %d"), ArenaManager->HeroUnits.Num());
	int32 RandomChance = FMath::RandRange(1, 100);
	(RandomChance <= GetRandomChanceByRound(CurrentRound)) ? TargetHero
		= SelectHeroWithLeastHealth() : TargetHero = RandomlySelectTarget();
	return TargetHero;
}

AHeroUnit* ADarkBobUnit::RandomlySelectTarget()
{
	UE_LOG(LogTemp, Log, TEXT("HeroList size: %d"), ArenaManager->HeroUnits.Num());
	if (ArenaManager->HeroUnits.Num() == 0)
	{
		return nullptr;
	}

	int32 RandomIndex = FMath::RandRange(0, ArenaManager->HeroUnits.Num() - 1);
	UE_LOG(LogTemp, Log, TEXT("Randomly selected hero: %s."), *ArenaManager->HeroUnits[RandomIndex]->GetName());
	return ArenaManager->HeroUnits[RandomIndex];
}

AHeroUnit* ADarkBobUnit::SelectHeroWithLeastHealth()
{
	UE_LOG(LogTemp, Log, TEXT("HeroList size: %d"), ArenaManager->HeroUnits.Num());
	if (ArenaManager->HeroUnits.Num() == 0)
		return nullptr;
	
	AHeroUnit* LowestHealthHero = ArenaManager->HeroUnits[0];
	for (AHeroUnit* Hero : ArenaManager->HeroUnits)
		if (Hero->CurrentHealth < LowestHealthHero->CurrentHealth)
			LowestHealthHero = Hero;
	UE_LOG(LogTemp, Log, TEXT("Selected hero with lowest health: %s."), *LowestHealthHero->GetName());
	return LowestHealthHero;
}

int32 ADarkBobUnit::GetRandomChanceByRound(int32 CurrentRounds) const
{
	return (CurrentRounds <= 3) ? PhaseOneRndHeroChance : PhaseTwoRndHeroChance;
}

void ADarkBobUnit::ExecuteNextAction()
{
	if (Actions.Num() > 0)
	{
		AHeroUnit* Teddy = bIsTaunted ? FindHeroByName(TEXT("Teddy")) : nullptr;
		
		FBossActionDB SelectedAction = Actions[0];
		UE_LOG(LogTemp, Log, TEXT("Boss executes action: Priority %d, Action: %d"), SelectedAction.Priority, SelectedAction.ActionType);

		switch (SelectedAction.ActionType)
		{
		case EBossActionTypeDB::BasicAttack:
			TargetHero = Teddy ? Teddy : SelectTarget();
			if (TargetHero) { BasicAttack(); }
			else { UE_LOG(LogTemp, Warning, TEXT("Hitta inte hero!")); }
			LastActionType = EBossActionTypeDB::BasicAttack;
			break;

		case EBossActionTypeDB::DarkBobSpecial:
			TargetHero = Teddy ? Teddy : SelectHeroWithLeastHealth(); 
			if (TargetHero) { DarkBobSpecial(); } else{UE_LOG(LogTemp, Warning, TEXT("Hitta inte hero!"));}
			CurrentMana = 0;
			LastActionType = EBossActionTypeDB::DarkBobSpecial;
			break;

		case EBossActionTypeDB::Charge:
			Charge();
			(Settings) ? CurrentMana += Settings->GetDarkBobChargeManaGain() : CurrentMana += ChargeManaGain;
			//CurrentMana++;
			LastActionType = EBossActionTypeDB::Charge;
			break;
		
		default:
			UE_LOG(LogTemp, Warning, TEXT("Unknown action type"));
			LastActionType = SelectedAction.ActionType;
			break;
		}

		OnAbilityInputExecuted.Broadcast(this);
	}
}

void ADarkBobUnit::OnSettingsChanged(FName SettingName, float NewValue)
{
	if (SettingName == "DarkBob_MaxMana")
	{
		MaxMana = (int)NewValue;
		UE_LOG(LogTemp, Warning, TEXT("DarkBob: MaxMana updated to %d"), MaxMana);
	}
	else if (SettingName == "DarkBob_ChargeManaGain")
	{
		ChargeManaGain = (int)NewValue;
		UE_LOG(LogTemp, Warning, TEXT("DarkBob: ChargeManaGain updated to %d"), ChargeManaGain);
	}
}

AHeroUnit* ADarkBobUnit::FindHeroByName(const FString& Name) const
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

void ADarkBobUnit::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

void ADarkBobUnit::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
}

void ADarkBobUnit::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UProjAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	InitializeDefaultAttributes();
}

void ADarkBobUnit::OnTargeted_Implementation()
{
	if(TargetDecal)
	{
		TargetDecal->SetVisibility(true);
		UE_LOG(LogTemp, Display, TEXT("OnTargeted"));
	}
}

void ADarkBobUnit::OnTargetedEnd_Implementation()
{
	if(TargetDecal)
	{
		UE_LOG(LogTemp, Display, TEXT("OnTargetedEnd"));
		TargetDecal->SetVisibility(false);
	}
}

int32 ADarkBobUnit::GetPlayerLevel()
{
	return Level;
}

void ADarkBobUnit::Die()
{
	ArenaManager->EnemyUnits.Remove(this);
	UE_LOG(LogTemp, Display, TEXT("Dies"));
	PlayDeathAnimation();
	Super::Die();
}

