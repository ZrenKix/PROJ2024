// Fill out your copyright notice in the Description page of Project Settings.


#include "Arena/Unit/HeroUnit.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/ProjAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnitConversion.h"
#include "Player/ProjPlayerController.h"
#include "Player/ProjPlayerState.h"
#include "Player/Controllers/ArenaPlayerController.h"
#include "UI/HUD/PlayerHUD.h"

AHeroUnit::AHeroUnit()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	
}

void AHeroUnit::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init Ability actor info for the server
	InitAbilityActorInfo();
	AddDefaultAbilities();
}

void AHeroUnit::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init Ability actor info for the client
	InitAbilityActorInfo();
}

bool AHeroUnit::ActionTurn()
{
	// Posses this
	AArenaPlayerController* PC = Cast<AArenaPlayerController>(GetController());
	PC->Possess(this);
	
	// Stuff
	UE_LOG(LogTemp, Warning, TEXT("HeroUnit::ActionTurn"));
	FPlatformProcess::Sleep(2);
	
	// Unposses current
	PC->UnPossess();
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

void AHeroUnit::InitAbilityActorInfo()
{
	AProjPlayerState* ProjPlayerState = GetPlayerState<AProjPlayerState>();
	check(ProjPlayerState);
	ProjPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(ProjPlayerState, this);
	Cast<UProjAbilitySystemComponent>(ProjPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = ProjPlayerState->GetAbilitySystemComponent();
	AttributeSet = ProjPlayerState->GetAttributeSet();

	if(AProjPlayerController* PlayerController = Cast<AProjPlayerController>(GetController()))
	{
		if(APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD()))
		{
			PlayerHUD->InitOverlay(PlayerController, ProjPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}

bool AHeroUnit::IsDead() const
{
	if (AttributeSet)
	{
		return Cast<UProjAttributeSet>(AttributeSet)->GetHealth() <= 0;
	}
	UE_LOG(LogTemp, Error, TEXT("%s: IsDead() failed to get attribute set"), *GetName());
	return false;
}

int32 AHeroUnit::GetPlayerLevel()
{
	const AProjPlayerState* ProjPlayerState = GetPlayerState<AProjPlayerState>();
	check(ProjPlayerState);
	return ProjPlayerState->GetPlayerLevel();
}
