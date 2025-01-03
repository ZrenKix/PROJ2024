// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EnemySettings.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEnemySettingChanged,
	FName, SettingName,
	float, NewValue
);

UCLASS(Blueprintable, BlueprintType)
class PROJ_API UEnemySettings : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category="EnemySettings|Delegates")
	FOnEnemySettingChanged OnEnemySettingChanged;
	
	// ----- DarkBob -----
    UFUNCTION(BlueprintCallable, Category="EnemySettings|DarkBob")
    int32 GetDarkBobMaxMana() const;

    UFUNCTION(BlueprintCallable, Category="EnemySettings|DarkBob")
    void SetDarkBobMaxMana(int32 NewValue);

    UFUNCTION(BlueprintCallable, Category="EnemySettings|DarkBob")
    int32 GetDarkBobChargeManaGain() const;

    UFUNCTION(BlueprintCallable, Category="EnemySettings|DarkBob")
    void SetDarkBobChargeManaGain(int32 NewValue);

    UFUNCTION(BlueprintCallable, Category="EnemySettings|DarkBob")
    float GetDarkBobDoubleQueueChance() const;

    UFUNCTION(BlueprintCallable, Category="EnemySettings|DarkBob")
    void SetDarkBobDoubleQueueChance(float NewValue);


    // ----- Erebus -----
    UFUNCTION(BlueprintCallable, Category="EnemySettings|Erebus")
    int32 GetErebusMaxMana() const;

    UFUNCTION(BlueprintCallable, Category="EnemySettings|Erebus")
    void SetErebusMaxMana(int32 NewValue);

    UFUNCTION(BlueprintCallable, Category="EnemySettings|Erebus")
    int32 GetErebusChargeManaGain() const;

    UFUNCTION(BlueprintCallable, Category="EnemySettings|Erebus")
    void SetErebusChargeManaGain(int32 NewValue);

    UFUNCTION(BlueprintCallable, Category="EnemySettings|Erebus")
    int32 GetErebusFailedMinionSpawnRoundIncrease() const;

    UFUNCTION(BlueprintCallable, Category="EnemySettings|Erebus")
    void SetErebusFailedMinionSpawnRoundIncrease(int32 NewValue);

    UFUNCTION(BlueprintCallable, Category="EnemySettings|Erebus")
    float GetErebusPhaseOneRndHeroChance() const;

    UFUNCTION(BlueprintCallable, Category="EnemySettings|Erebus")
    void SetErebusPhaseOneRndHeroChance(float NewValue);

    UFUNCTION(BlueprintCallable, Category="EnemySettings|Erebus")
    float GetErebusPhaseTwoRndHeroChance() const;

    UFUNCTION(BlueprintCallable, Category="EnemySettings|Erebus")
    void SetErebusPhaseTwoRndHeroChance(float NewValue);


    // ----- SmallShadow -----
    UFUNCTION(BlueprintCallable, Category="EnemySettings|SmallShadow")
    float GetSmallShadowRandomSelectChance() const;

    UFUNCTION(BlueprintCallable, Category="EnemySettings|SmallShadow")
    void SetSmallShadowRandomSelectChance(float NewValue);

private:

	// SmallShadowUnit
	UPROPERTY(EditAnywhere, Category="SmallShadow")
	float SmallShadow_RandomSelectChance = 40.0f;

	// DarkBobUnit
	UPROPERTY(EditAnywhere, Category="DarkBob")
	int32 DarkBob_MaxMana = 2.0f;

	UPROPERTY(EditAnywhere, Category="DarkBob")
	int32 DarkBob_ChargeManaGain = 2.0f;

	UPROPERTY(EditAnywhere, Category="DarkBob")
	float DarkBob_DoubleQueueChance = 50.0f; 

	// ErebusBossUnit
	UPROPERTY(EditAnywhere, Category="Erebus")
	int32 Erebus_MaxMana = 4.0f;

	UPROPERTY(EditAnywhere, Category="Erebus")
	int32 Erebus_ChargeManaGain = 2.0f;

	UPROPERTY(EditAnywhere, Category="Erebus")
	int32 Erebus_FailedMinionSpawnRoundIncrease = 3.0f; 

	UPROPERTY(EditAnywhere, Category="Erebus")
	float Erebus_PhaseOneRndHeroChance = 40.0f;

	UPROPERTY(EditAnywhere, Category="Erebus")
	float Erebus_PhaseTwoRndHeroChance = 25.0f;
};
