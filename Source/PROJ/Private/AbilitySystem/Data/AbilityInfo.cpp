// © 2024 Choklad4Life. All rights reserved.


#include "AbilitySystem/Data/AbilityInfo.h"

FDBAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag AbilityTag, bool bLogNotFound) const
{
	for(const FDBAbilityInfo& Info : AbilityInformation)
	{
		if(Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}

	if(bLogNotFound)
	{
		UE_LOG(LogTemp, Display, TEXT("Ability Tag not found"));
	}

	return FDBAbilityInfo();
}
