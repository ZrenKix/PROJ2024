// Fill out your copyright notice in the Description page of Project Settings.


#include "Arena/Unit//UnitBase.h"

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

int AUnitBase::GainXp(int Amount)
{
	return 1;
}

bool AUnitBase::LevelUp()
{
	return true;
}






