// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ProjCharacterBase.h"

// Sets default values
AProjCharacterBase::AProjCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AProjCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}