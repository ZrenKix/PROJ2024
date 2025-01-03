// © 2024 Choklad4Life. All rights reserved.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/ProjAbilitySystemComponent.h"
#include "AbilitySystem/ProjAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UProjAttributeSet* ProjAttributeSet = CastChecked<UProjAttributeSet>(AttributeSet);
	
	OnHealthChanged.Broadcast(ProjAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(ProjAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(ProjAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(ProjAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UProjAttributeSet* ProjAttributeSet = CastChecked<UProjAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		ProjAttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		ProjAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		ProjAttributeSet->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		ProjAttributeSet->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
		);
	
	if(UProjAbilitySystemComponent* ProjASC = Cast<UProjAbilitySystemComponent>(AbilitySystemComponent))
	{
		if(ProjASC->bStartupAbilitiesGiven)
		{
			OnInitializeStartupAbilities(ProjASC);
		}
		else
		{
			ProjASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		}

		
		ProjASC->OnEffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for(const FGameplayTag& Tag : AssetTags)
			{
				// For example, say that Tag = Message.HealthPotion
				// "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False
				// So, we can use this to filter out the tags we want
				if(FGameplayTag MessageTag =  FGameplayTag::RequestGameplayTag(FName("Message")); Tag.MatchesTag(MessageTag))
				{
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					OnMessageWidgetRow.Broadcast(*Row);
				}

			}
		}
	);
	}

	
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UProjAbilitySystemComponent* ProjAbilitySystemComponent)
{
	//Implement
	if(!ProjAbilitySystemComponent->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, ProjAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
	{
		FDBAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(ProjAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = ProjAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	ProjAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}
