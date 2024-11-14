// Fill out your copyright notice in the Description page of Project Settings.


#include "Arena/Unit//UnitBase.h"

#include "AbilitySystem/ProjAbilitySystemComponent.h"

// Sets default values
AUnitBase::AUnitBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AUnitBase::BeginPlay()
{
	Super::BeginPlay();
}

void AUnitBase::InitAbilityActorInfo()
{
}

void AUnitBase::AddDefaultAbilities()
{

	UProjAbilitySystemComponent* UnitASC = CastChecked<UProjAbilitySystemComponent>(AbilitySystemComponent);
	if(!HasAuthority()) return;

	UnitASC->AddCharacterAbilities(DefaultAbilities);
}

int AUnitBase::GainXp(int Amount)
{
	return 1;
}

bool AUnitBase::LevelUp()
{
	return true;
}






