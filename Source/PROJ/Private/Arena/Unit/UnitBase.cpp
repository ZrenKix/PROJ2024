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

bool AUnitBase::ActionTurn()
{
	return true;
}

int AUnitBase::GainXp(int Amount)
{
	return 1;
}

bool AUnitBase::LevelUp()
{
	return true;
}

void AUnitBase::OnDeath()
{
	UE_LOG(LogTemp, Display, TEXT("OnDeath"));
}






