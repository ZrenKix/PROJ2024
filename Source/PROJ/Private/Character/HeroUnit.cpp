// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/HeroUnit.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/ProjPlayerController.h"
#include "Player/ProjPlayerState.h"
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
}

void AHeroUnit::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init Ability actor info for the client
	InitAbilityActorInfo();
}

bool AHeroUnit::ActionTurn()
{
	return Super::ActionTurn();
}

int AHeroUnit::GainXp(int Amount)
{
	return Super::GainXp(Amount);
}

bool AHeroUnit::LevelUp()
{
	return Super::LevelUp();
}

void AHeroUnit::InitAbilityActorInfo()
{
	AProjPlayerState* ProjPlayerState = GetPlayerState<AProjPlayerState>();
	check(ProjPlayerState);
	ProjPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(ProjPlayerState, this);
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
