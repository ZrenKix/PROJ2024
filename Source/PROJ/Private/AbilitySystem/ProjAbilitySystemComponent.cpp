// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ProjAbilitySystemComponent.h"

void UProjAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UProjAbilitySystemComponent::EffectApplied);
}

void UProjAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("Effect applied"));
}
