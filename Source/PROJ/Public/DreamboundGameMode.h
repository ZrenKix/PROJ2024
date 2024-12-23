// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "GameFramework/GameModeBase.h"
#include "DreamboundGameMode.generated.h"

UCLASS()
class PROJ_API ADreamboundGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADreamboundGameMode();
	
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
	
public:
	void SwitchToArenaController();
	void SwitchToWorldController();

private:
	//AArenaPlayerController* ArenaController;
	//AProjPlayerController* WorldController;
};
