// © 2024 Choklad4Life. All rights reserved.


#include "AbilitySystem/DBAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "DreamboundGameMode.h"
#include "Engine/OverlapResult.h"
#include "Game/ProjGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ProjPlayerState.h"
#include "UI/HUD/PlayerHUD.h"
#include "UI/WidgetController/DBWidgetController.h"

UOverlayWidgetController* UDBAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if(APlayerController* PC =UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if(APlayerHUD* HUD = Cast<APlayerHUD>(PC->GetHUD()))
		{
			AProjPlayerState* PS = PC->GetPlayerState<AProjPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AttributeSet = PS->GetAttributeSet();
			const FWidgetControllerParams Params = FWidgetControllerParams(PC, PS, ASC, AttributeSet);
			return HUD->GetOverlayWidgetController(Params);
		}
	}
	return nullptr;
}

void UDBAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
														   TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
														   const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

void UDBAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	const AActor* AvatarActor = ASC->GetAvatarActor();

	const UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	const FCharacterClassDefaultInfo DefaultInfo = CharacterClassInfo->GetClassDefaults(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContext = ASC->MakeEffectContext();
	PrimaryAttributesContext.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(DefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContext);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContext = ASC->MakeEffectContext();
	SecondaryAttributesContext.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContext);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContext = ASC->MakeEffectContext();
	VitalAttributesContext.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContext);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UDBAbilitySystemLibrary::GiveStartupAbilites(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	for(TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
}

UCharacterClassInfo* UDBAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AProjGameModeBase* GameModeBase = Cast<AProjGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameModeBase != nullptr)
	{
		return GameModeBase->CharacterClassInfo;
	}
	else
	{
		ADreamboundGameMode* DreamboundGameMode = Cast<ADreamboundGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
		if (DreamboundGameMode != nullptr)
		{
			return DreamboundGameMode->CharacterClassInfo;
		}
	}

	return nullptr;
}
