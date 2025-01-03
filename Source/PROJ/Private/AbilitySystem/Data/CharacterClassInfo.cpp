// © 2024 Choklad4Life. All rights reserved.


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaults(ECharacterClass CharacterClass) const
{
	return CharacterClassInfo.FindChecked(CharacterClass);
}
