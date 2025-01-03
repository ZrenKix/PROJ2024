// © 2024 Choklad4Life. All rights reserved.


#include "Input/DBInputConfig.h"

const UInputAction* UDBInputConfig::FindAbilityInputActionForTag(const FGameplayTag& Tag, bool bLogNotFound) const
{
	for (const FDBInputAction& Action : AbilityInputActions)
	{
		if(Action.InputAction && Action.InputTag.MatchesTag(Tag))
		{
			return Action.InputAction;
		}
	}

	if(bLogNotFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("No input action found for tag %s"), *Tag.ToString());
	}

	return nullptr;
}
