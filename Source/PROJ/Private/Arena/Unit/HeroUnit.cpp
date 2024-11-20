// © 2024 Choklad4Life. All rights reserved.


#include "Arena/Unit/HeroUnit.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/ProjAbilitySystemComponent.h"
#include "Player/Controllers/ArenaPlayerController.h"
#include "UI/HUD/PlayerHUD.h"

AHeroUnit::AHeroUnit()
{
	AbilitySystemComponent = CreateDefaultSubobject<UProjAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // Kolla https://imgur.com/a/qExGmrP för mer info


	AttributeSet = CreateDefaultSubobject<UProjAttributeSet>(TEXT("AttributeSet"));
}

void AHeroUnit::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Enable input for the character
	if (AArenaPlayerController* PC = Cast<AArenaPlayerController>(NewController))
	{
		UE_LOG(LogTemp, Display, TEXT("Enabled PlayerController"));
		EnableInput(PC);
	}
}

void AHeroUnit::UnPossessed()
{
	Super::UnPossessed();

	// Disable input when unpossessed
	DisableInput(nullptr);
}

void AHeroUnit::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
	AddDefaultAbilities();
}

void AHeroUnit::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UProjAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	InitializeDefaultAttributes();

	if(AArenaPlayerController* PlayerController = Cast<AArenaPlayerController>(GetController()))
	{
		if(APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD()))
		{
			PlayerHUD->InitOverlay(PlayerController, GetPlayerState(), AbilitySystemComponent, AttributeSet);
		}
	}
}


void AHeroUnit::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	InitAbilityActorInfo();
}

bool AHeroUnit::ActionTurn()
{
	if (IsDead())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is dead"), *GetName());
		return false;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("HeroUnit::ActionTurn"));

	return true;
}

int AHeroUnit::GainXp(int Amount)
{
	return Super::GainXp(Amount);
}

bool AHeroUnit::LevelUp()
{
	return Super::LevelUp();
}

void AHeroUnit::OnDeath()
{
	Super::OnDeath();
}

int32 AHeroUnit::GetPlayerLevel()
{
	return 1; 
}