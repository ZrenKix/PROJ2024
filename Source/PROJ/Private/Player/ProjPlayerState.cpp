// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ProjPlayerState.h"

#include "AbilitySystem/ProjAbilitySystemComponent.h"
#include "AbilitySystem/ProjAttributeSet.h"

AProjPlayerState::AProjPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UProjAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UProjAttributeSet>(TEXT("AttributeSet"));
	
	// Updaterar till servern om man vill ha multiplayer, utvecklar för det även om det ej är multiplayer
	NetUpdateFrequency = 100.f;
}
