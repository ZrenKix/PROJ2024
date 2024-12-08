// © 2024 Choklad4Life. All rights reserved.


#include "Arena/Unit/DarkBobUnit.h"

#include "DBGameplayTags.h"
#include "AbilitySystem/DBAbilitySystemLibrary.h"
#include "AbilitySystem/ProjAttributeSet.h"
#include "Arena/ArenaManager.h"
#include "Arena/Unit/HeroUnit.h"
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
}

void ADarkBobUnit::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	AddDefaultAbilities();
	UDBAbilitySystemLibrary::GiveStartupAbilites(this, AbilitySystemComponent);
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
	if (bOnCooldown)
	{
		Actions.Add(FBossActionDB(1, EBossActionTypeDB::CoolDown));
	}

	if (CurrentRound >= nextRoundThreshold)
	{
		Actions.Add(FBossActionDB(2, EBossActionTypeDB::SpawnMinions));
	}

	if (CurrentMana >= MaxMana)
	{
		Actions.Add(FBossActionDB(3, EBossActionTypeDB::AoEAttack));
	}

	if (CurrentMana < MaxMana)
	{
		Actions.Add(FBossActionDB(4, EBossActionTypeDB::Charge));
		Actions.Add(FBossActionDB(4, EBossActionTypeDB::BasicAttack));
	}
	
	Actions.Sort([](const FBossActionDB& A, const FBossActionDB& B)
	{
		if (A.Priority == B.Priority)
		{
			return FMath::RandBool();
		}
		return A.Priority < B.Priority;
	});
	GetWorld()->GetTimerManager().SetTimer(BeforeAttackDelay, this, &ADarkBobUnit::ExecuteNextAction, 1.0f, false);

	CurrentRound++;
	return true;
}

AHeroUnit* ADarkBobUnit::SelectTarget()
{
	return nullptr;
}

AHeroUnit* ADarkBobUnit::RandomlySelectTarget()
{
	if (HeroList.Num() == 0)
	{
		return nullptr;
	}

	int32 RandomIndex = FMath::RandRange(0, HeroList.Num() - 1);
	UE_LOG(LogTemp, Log, TEXT("Randomly selected hero: %s."), *HeroList[RandomIndex]->GetName());
	return HeroList[RandomIndex];
}

AHeroUnit* ADarkBobUnit::SelectHeroWithLeastHealth()
{
	if (HeroList.Num() == 0)
		return nullptr;
	
	AHeroUnit* LowestHealthHero = HeroList[0];
	for (AHeroUnit* Hero : HeroList)
		if (Hero->CurrentHealth < LowestHealthHero->CurrentHealth)
			LowestHealthHero = Hero;
	UE_LOG(LogTemp, Log, TEXT("Selected hero with lowest health: %s."), *LowestHealthHero->GetName());
	return LowestHealthHero;
}

int32 ADarkBobUnit::GetRandomChanceByRound(int32 CurrentRounds) const
{
	return (CurrentRounds <= 6) ? PhaseOneRndHeroChance : PhaseTwoRndHeroChance;
}

void ADarkBobUnit::ExecuteNextAction()
{
	if (Actions.Num() > 0)
	{
		FBossActionDB SelectedAction = Actions[0];
		UE_LOG(LogTemp, Log, TEXT("Boss executes action: Priority %d"), SelectedAction.Priority);

		switch (SelectedAction.ActionType)
		{
		case EBossActionTypeDB::CoolDown:
			CoolDown();
			bOnCooldown = false;
			break;

		case EBossActionTypeDB::SpawnMinions:
			UE_LOG(LogTemp, Log, TEXT("Erebus spawns minions."));
			if (ArenaManager)
			{
				bool bSpawnedMinions = ArenaManager->SpawnEnemies(ArenaManager->PendingEnemyClasses, 0, 2);
				nextRoundThreshold += bSpawnedMinions ? 3 : 3;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Cannot spawn minions: ArenaManager is null"));
			}
			
			break;

		case EBossActionTypeDB::AoEAttack:
			AoeAttack();
			CurrentMana = 0;
			bOnCooldown = true;
			break;

		case EBossActionTypeDB::Charge:
			Charge();
			CurrentMana += 2;
			break;

		case EBossActionTypeDB::BasicAttack:
			TargetHero = SelectTarget();
			if (TargetHero)
			{
				BasicAttack();
			}
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("Unknown action type"));
			break;
		}

		OnAbilityInputExecuted.Broadcast(this);
	}
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
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
}

void ADarkBobUnit::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UProjAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	InitializeDefaultAttributes();
}

void ADarkBobUnit::OnTargeted_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void ADarkBobUnit::OnTargetedEnd_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
}

int32 ADarkBobUnit::GetPlayerLevel()
{
	return Level;
}

void ADarkBobUnit::Die()
{
	ArenaManager->EnemyUnits.Remove(this);
	UE_LOG(LogTemp, Display, TEXT("Dies"));
	SetLifeSpan(LifeSpan);
	Super::Die();
}

