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
	UE_LOG(LogTemp, Warning, TEXT("PossessedBy AHeroUnit"));
	Super::PossessedBy(NewController);
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

void AHeroUnit::InitAbilityActorInfo()
{
	AProjPlayerState* ProjPlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<AProjPlayerState>();
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

void AHeroUnit::BeginPlay()
{
	InitAbilityActorInfo();
	AddDefaultAbilities();

	if (AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has AttributeSet"), *GetName());
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is missing AttributeSet"), *GetName());
	}
	
	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has AbilitySystemComponent"), *GetName());
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is missing AbilitySystemComponent"), *GetName());
	}
	
	Super::BeginPlay();
}

int32 AHeroUnit::GetPlayerLevel()
{
	const AProjPlayerState* ProjPlayerState = GetPlayerState<AProjPlayerState>();
	check(ProjPlayerState);
	return ProjPlayerState->GetPlayerLevel();
}
