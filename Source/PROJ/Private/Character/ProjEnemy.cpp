// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ProjEnemy.h"
#include "AbilitySystem/ProjAbilitySystemComponent.h"
#include "AbilitySystem/ProjAttributeSet.h"
#include "PROJ/PROJ.h"

AProjEnemy::AProjEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UProjAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // Kolla https://imgur.com/a/qExGmrP för mer info


	AttributeSet = CreateDefaultSubobject<UProjAttributeSet>(TEXT("AttributeSet"));
	
}

void AProjEnemy::OnTargeted()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AProjEnemy::OnTargetedEnd()
{
	GetMesh()->SetRenderCustomDepth(false);
}

void AProjEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
}

void AProjEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UProjAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
}
