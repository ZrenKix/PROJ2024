// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ProjCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/ProjPlayerState.h"

AProjCharacter::AProjCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	
}

void AProjCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init Ability actor info for the server
	//InitAbilityActorInfo();
}

void AProjCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init Ability actor info for the client
	//InitAbilityActorInfo();
}

void AProjCharacter::InitAbilityActorInfo()
{
	AProjPlayerState* ProjPlayerState = GetPlayerState<AProjPlayerState>();
	check(ProjPlayerState);
	ProjPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(ProjPlayerState, this);
	AbilitySystemComponent = ProjPlayerState->GetAbilitySystemComponent();
	AttributeSet = ProjPlayerState->GetAttributeSet();
}
