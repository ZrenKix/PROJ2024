// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ProjPlayerState.h"

#include "AbilitySystem/ProjAbilitySystemComponent.h"
#include "AbilitySystem/ProjAttributeSet.h"
#include "Net/UnrealNetwork.h"

AProjPlayerState::AProjPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UProjAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // Kolla https://imgur.com/a/qExGmrP för mer info

	AttributeSet = CreateDefaultSubobject<UProjAttributeSet>(TEXT("AttributeSet"));
	
	// Updaterar till servern om man vill ha multiplayer, utvecklar för det även om det ej är multiplayer
	NetUpdateFrequency = 100.f;
}

void AProjPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjPlayerState, Level);
}

void AProjPlayerState::OnRep_Level(int32 OldLevel)
{
	
}
