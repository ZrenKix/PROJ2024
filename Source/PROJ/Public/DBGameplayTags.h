// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * DBGameplayTags
 *
 * Singleton containing native Gameplay Tags
 */
struct FDBGameplayTags
{
public:
 static const FDBGameplayTags& Get() {return GameplayTags;}
 static void InitializeNativeGameplayTags();

 FGameplayTag Attributes_Primary_Strength;
 FGameplayTag Attributes_Primary_Initiative;
 
 FGameplayTag Attributes_Secondary_Armor;
 FGameplayTag Attributes_Secondary_MaxHealth;
 FGameplayTag Attributes_Secondary_MaxMana;

 FGameplayTag InputTag_LMB;
 FGameplayTag InputTag_RMB;
 FGameplayTag InputTag_1;
 FGameplayTag InputTag_2;
 FGameplayTag InputTag_3;
 FGameplayTag InputTag_4;

 FGameplayTag Damage;
 FGameplayTag Damage_Fire;

 FGameplayTag Abilities_Arrow_Basic;
 FGameplayTag Abilities_Arrow_Rain;

 TArray<FGameplayTag> DamageTypes;
 
 FGameplayTag Effects_HitReact;


protected:
 
private:
 static FDBGameplayTags GameplayTags;
};
