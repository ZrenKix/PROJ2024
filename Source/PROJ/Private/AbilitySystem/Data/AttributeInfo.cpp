// © 2024 Choklad4Life. All rights reserved.


#include "AbilitySystem/Data/AttributeInfo.h"

FDBAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag Tag, bool bLogNotFound) const
{
	for(const FDBAttributeInfo& Info : AttributeInfos)
	{
		if(Info.AttributeTag.MatchesTagExact(Tag))
		{
			return Info;
		}
	}

	if(bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("No such info"));
	}

	return FDBAttributeInfo();
}
