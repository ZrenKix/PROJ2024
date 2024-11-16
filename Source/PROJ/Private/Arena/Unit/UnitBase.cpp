// Fill out your copyright notice in the Description page of Project Settings.


#include "Arena/Unit//UnitBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ProjAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PROJ/PROJ.h"

// Sets default values
AUnitBase::AUnitBase()
{
	PrimaryActorTick.bCanEverTick = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
}

void AUnitBase::BeginPlay()
{
	Super::BeginPlay();
}

void AUnitBase::InitAbilityActorInfo()
{
}

void AUnitBase::InitializePrimaryAttributes() const
{
	check(IsValid(GetAbilitySystemComponent()));
	//check(IsValid(DefaultPrimaryAttributes));
	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultPrimaryAttributes, 1.f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AUnitBase::AddDefaultAbilities()
{

	UProjAbilitySystemComponent* UnitASC = CastChecked<UProjAbilitySystemComponent>(AbilitySystemComponent);
	if(!HasAuthority()) return;

	UnitASC->AddCharacterAbilities(DefaultAbilities);
}

FVector AUnitBase::GetPlayerLocation()
{
	// Get the Player Character (assuming the player is the default player character)
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter)
	{
		return PlayerCharacter->GetActorLocation();
	}

	// Return a default value if the player character isn't found
	return FVector::ZeroVector;
}



int AUnitBase::GainXp(int Amount)
{
	return 1;
}

bool AUnitBase::LevelUp()
{
	return true;
}






