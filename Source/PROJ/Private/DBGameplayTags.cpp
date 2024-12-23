// © 2024 Choklad4Life. All rights reserved.


#include "DBGameplayTags.h"
#include "GameplayTagsManager.h"

FDBGameplayTags FDBGameplayTags::GameplayTags;

void FDBGameplayTags::InitializeNativeGameplayTags()
{
	// Initialize native gameplay tags here
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("Increases damage done")
		);

	GameplayTags.Attributes_Primary_Initiative = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Initiative"),
		FString("Higher initiative equals bigger chance to start in battle")
		);

	//Not really using, just for playtest
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),
		FString("Reduces damage taken")
		);

	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"),
		FString("Increases Max Health")
		);

	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"),
		FString("Increases Max Mana")
		);

	/*
	 * Input Tags
	 */

	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"),
		FString("Input Tag for Left Mouse Button")
		);

	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("Input Tag for Right Mouse Button")
		);

	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.1"),
		FString("Input Tag for 1 key")
		);

	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("InputTag.2"),
	FString("Input Tag for 2 key")
	);

	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.3"),
		FString("Input Tag for 3 key")
		);

	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.4"),
		FString("Input Tag for 4 key")
		);

	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage"),
		FString("Damage")
		);

	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Fire"),
		FString("Fire Damage Type")
		);

	/*
	 * Offensive Abilities
	 */

	GameplayTags.Abilities_Arrow_Rain = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Arrow.Rain"),
		FString("Rain of arrows ability Tag")
		);

	GameplayTags.Abilities_Arrow_Basic = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Arrow.Basic"),
		FString("Basic arrow ability Tag")
		);

	GameplayTags.DamageTypes.Add(GameplayTags.Damage_Fire);

	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Effects_HitReact"),
		FString("Tag granted when hit reacting")
		);
}
