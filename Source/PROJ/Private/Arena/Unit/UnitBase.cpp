// Fill out your copyright notice in the Description page of Project Settings.


#include "Arena/Unit/UnitBase.h"
#include "AbilitySystemComponent.h"
#include "VectorUtil.h"
#include "AbilitySystem/ProjAbilitySystemComponent.h"
#include "Arena/ArenaManager.h"
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

void AUnitBase::Die()
{
	MulticastHandleDeath();
	Death.Broadcast();

	UArenaManager* ArenaManager = GetWorld()->GetSubsystem<UArenaManager>();
	ArenaManager->RemoveUnit(this);
}

void AUnitBase::MulticastHandleDeath_Implementation()
{
	//GetMesh()->SetEnableGravity(true);
	//GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

AActor* AUnitBase::GetAvatar_Implementation()
{
	return this;
}

bool AUnitBase::IsDead_Implementation() const
{
	return bDead;
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

void AUnitBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	//check(IsValid(EffectClass));
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(EffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AUnitBase::InitializeDefaultAttributes() const
{ 
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}


int AUnitBase::GainXp(int Amount)
{
	return 1;
}

bool AUnitBase::LevelUp()
{
	return true;
}

bool AUnitBase::EnoughManaFor(int value) const
{
	return CurrentMana >= value;
}

void AUnitBase::ChangeMana(int value)
{
	CurrentMana += value;
	CurrentMana = FMath::Clamp(CurrentMana, 0, MaxMana);
}






