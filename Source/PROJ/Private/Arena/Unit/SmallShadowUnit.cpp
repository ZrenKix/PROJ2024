// © 2024 Choklad4Life. All rights reserved.


#include "Arena/Unit/SmallShadowUnit.h"

#include "DBGameplayTags.h"
#include "EnemySettings.h"
#include "AbilitySystem/DBAbilitySystemLibrary.h"
#include "AbilitySystem/ProjAttributeSet.h"
#include "Arena/ArenaManager.h"
#include "Arena/Unit/HeroUnit.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/DBUserWidget.h"

ASmallShadowUnit::ASmallShadowUnit()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UProjAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // Kolla https://imgur.com/a/qExGmrP för mer info
	
	AttributeSet = CreateDefaultSubobject<UProjAttributeSet>(TEXT("AttributeSet"));

	TargetDecal = CreateDefaultSubobject<UDecalComponent>("TargetDecal");
	TargetDecal->SetupAttachment(GetRootComponent());
	TargetDecal->SetVisibility(false);

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(GetRootComponent());
}

void ASmallShadowUnit::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	AddDefaultAbilities();
	UDBAbilitySystemLibrary::GiveStartupAbilites(this, AbilitySystemComponent);

	if (!ArenaManager)
	{
		ArenaManager = GetWorld()->GetSubsystem<UArenaManager>();
	}
	Settings = GetWorld()->GetGameInstance()->GetSubsystem<UEnemySettings>();
	if (Settings)
	{
		Chance = Settings->GetSmallShadowRandomSelectChance();
		UE_LOG(LogTemp, Warning, TEXT("ASmallShadowUnit::BeginPlay() - Initial SmallShadow_RandomSelectChance = %f"), Chance);

		Settings->OnEnemySettingChanged.AddDynamic(this, &ASmallShadowUnit::OnSettingsChanged);
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
			&ASmallShadowUnit::HitReactTagChanged
		);
		
		OnHealthChanged.Broadcast(AS->GetHealth());
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	}
}

void ASmallShadowUnit::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
}

void ASmallShadowUnit::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UProjAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	InitializeDefaultAttributes();
}

void ASmallShadowUnit::OnSettingsChanged(FName SettingName, float NewValue)
{
	if (SettingName == "SmallShadow_RandomSelectChance")
	{
		Chance = NewValue;
		UE_LOG(LogTemp, Warning, TEXT("ASmallShadowUnit::OnSettingsChanged - RandomSelectChance updated to %f"), Chance);
	}
}

void ASmallShadowUnit::OnTargeted_Implementation()
{
	if(TargetDecal)
	{
		TargetDecal->SetVisibility(true);
		UE_LOG(LogTemp, Display, TEXT("OnTargeted"));
	}
}

void ASmallShadowUnit::OnTargetedEnd_Implementation()
{
	if(TargetDecal)
	{
		UE_LOG(LogTemp, Display, TEXT("OnTargetedEnd"));
		TargetDecal->SetVisibility(false);
	}
}

int32 ASmallShadowUnit::GetPlayerLevel()
{
	return Level;
}


void ASmallShadowUnit::Die()
{
	ArenaManager->EnemyUnits.Remove(this);
	ArenaManager->SpawnedEnemies.Remove(this);
	UE_LOG(LogTemp, Display, TEXT("Dies"));
	PlayDeathAnimation();
	Super::Die();
}

AHeroUnit* ASmallShadowUnit::FindHeroByName(const FString& Name) const
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

void ASmallShadowUnit::NotifyPlayerOfDeath()
{
	// Display a message to the player
	if (GEngine)
	{
		FString DeathMessage = FString::Printf(TEXT("%s has been defeated!"), *GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, DeathMessage);
	}

	// Optionally, you can implement more sophisticated UI feedback here
}

bool ASmallShadowUnit::ActionTurn()
{
	UE_LOG(LogTemp, Log, TEXT("%s is preparing to attack..."), *GetName());
	
	GetWorld()->GetTimerManager().SetTimer(
		BeforeAttackDelay,
		[this]()
		{
			ArenaManager = GetWorld()->GetSubsystem<UArenaManager>();
			HeroList = ArenaManager->GetHeroes();

			if (HeroList.IsEmpty())
			{
				UE_LOG(LogTemp, Warning, TEXT("No alive heroes to attack."));
				return;
			}

			if (bIsTaunted)
			{
				for (AHeroUnit* Hero : ArenaManager->GetHeroes())
				{
					if (Hero->UnitName.Equals("Teddy"))
					{
						if (!Hero->IsDead()) TargetHero = Hero;
						else TargetHero = SelectTarget();
						EndTauntEffect();
						break;
					}
				}
			} else {
				TargetHero = SelectTarget();
			}

			if (!TargetHero)
			{
				UE_LOG(LogTemp, Warning, TEXT("Target selection failed."));
				return;
			}
			Attack();
		},
		1.0f,
		false 
	);
	CurrentRound++;
	return true;
}

AHeroUnit* ASmallShadowUnit::SelectTarget()
{
	if (!Settings) return RandomlySelectTarget();
	if (bIsFirstAttack)
	{
		bIsFirstAttack = false;
		return RandomlySelectTarget();
	}
	
	if (!ArenaManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("ArenaManager not found."));
		return nullptr;
	}

	int32 RandomChance = FMath::RandRange(1, 100);
	Chance = Settings->GetSmallShadowRandomSelectChance();

	return (RandomChance <= Chance)
		? RandomlySelectTarget()
		: SelectHeroWithLeastHealth();
}

AHeroUnit* ASmallShadowUnit::RandomlySelectTarget()
{
	if (HeroList.Num() == 0)
	{
		return nullptr;
	}

	int32 RandomIndex = FMath::RandRange(0, HeroList.Num() - 1);
	UE_LOG(LogTemp, Log, TEXT("Randomly selected hero: %s."), *HeroList[RandomIndex]->GetName());
	return HeroList[RandomIndex];
}

AHeroUnit* ASmallShadowUnit::SelectHeroWithLeastHealth()
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

int32 ASmallShadowUnit::GetRandomChanceByRound(int32 CurrentRounds) const
{
	return (CurrentRounds <= 5) ? 40 : 25;
}

bool ASmallShadowUnit::IsAlive() const
{
	return Health > 0.0f;
}

void ASmallShadowUnit::FinishedTurn()
{
	OnAbilityInputExecuted.Broadcast(this);
}