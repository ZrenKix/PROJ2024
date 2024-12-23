// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EnemySettings.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PROJ_API UEnemySettings : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:

	// SmallShadowUnit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SmallShadow")
	float SmallShadow_RandomSelectChance = 40.0f;

	// DarkBobUnit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DarkBob")
	int32 DarkBob_MaxMana = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DarkBob")
	int32 DarkBob_ChargeManaGain = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DarkBob")
	float DarkBob_DoubleQueueChance = 50.0f; 

	// ErebusBossUnit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Erebus")
	int32 Erebus_MaxMana = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Erebus")
	int32 Erebus_ChargeManaGain = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Erebus")
	int32 Erebus_FailedMinionSpawnRoundIncrease = 3.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Erebus")
	float Erebus_PhaseOneRndHeroChance = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Erebus")
	float Erebus_PhaseTwoRndHeroChance = 25.0f;
};
