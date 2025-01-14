// © 2024 Choklad4Life. All rights reserved.

#include "EnemySettings.h"

// *********************************
// *            DarkBob            *
// *********************************

int32 UEnemySettings::GetDarkBobMaxMana() const
{
    return DarkBob_MaxMana;
}

void UEnemySettings::SetDarkBobMaxMana(int32 NewValue)
{
    DarkBob_MaxMana = NewValue;
    OnEnemySettingChanged.Broadcast("DarkBob_MaxMana", static_cast<float>(NewValue));
}

int32 UEnemySettings::GetDarkBobChargeManaGain() const
{
    return DarkBob_ChargeManaGain;
}

void UEnemySettings::SetDarkBobChargeManaGain(int32 NewValue)
{
    DarkBob_ChargeManaGain = NewValue;
    OnEnemySettingChanged.Broadcast("DarkBob_ChargeManaGain", static_cast<float>(NewValue));
}

float UEnemySettings::GetDarkBobDoubleQueueChance() const
{
    return DarkBob_DoubleQueueChance;
}

void UEnemySettings::SetDarkBobDoubleQueueChance(float NewValue)
{
    DarkBob_DoubleQueueChance = NewValue;
    OnEnemySettingChanged.Broadcast("DarkBob_DoubleQueueChance", NewValue);
}


// *********************************
// *            Erebus             *
// *********************************

int32 UEnemySettings::GetErebusMaxMana() const
{
    return Erebus_MaxMana;
}

void UEnemySettings::SetErebusMaxMana(int32 NewValue)
{
    Erebus_MaxMana = NewValue;
    OnEnemySettingChanged.Broadcast("Erebus_MaxMana", static_cast<float>(NewValue));
}

int32 UEnemySettings::GetErebusChargeManaGain() const
{
    return Erebus_ChargeManaGain;
}

void UEnemySettings::SetErebusChargeManaGain(int32 NewValue)
{
    Erebus_ChargeManaGain = NewValue;
    OnEnemySettingChanged.Broadcast("Erebus_ChargeManaGain", static_cast<float>(NewValue));
}

int32 UEnemySettings::GetErebusFailedMinionSpawnRoundIncrease() const
{
    return Erebus_FailedMinionSpawnRoundIncrease;
}

void UEnemySettings::SetErebusFailedMinionSpawnRoundIncrease(int32 NewValue)
{
    Erebus_FailedMinionSpawnRoundIncrease = NewValue;
    OnEnemySettingChanged.Broadcast("Erebus_FailedMinionSpawnRoundIncrease", static_cast<float>(NewValue));
}

float UEnemySettings::GetErebusPhaseOneRndHeroChance() const
{
    return Erebus_PhaseOneRndHeroChance;
}

void UEnemySettings::SetErebusPhaseOneRndHeroChance(float NewValue)
{
    Erebus_PhaseOneRndHeroChance = NewValue;
    OnEnemySettingChanged.Broadcast("Erebus_PhaseOneRndHeroChance", NewValue);
}

float UEnemySettings::GetErebusPhaseTwoRndHeroChance() const
{
    return Erebus_PhaseTwoRndHeroChance;
}

void UEnemySettings::SetErebusPhaseTwoRndHeroChance(float NewValue)
{
    Erebus_PhaseTwoRndHeroChance = NewValue;
    OnEnemySettingChanged.Broadcast("Erebus_PhaseTwoRndHeroChance", NewValue);
}


// *********************************
// *        SmallShadowUnit        *
// *********************************

float UEnemySettings::GetSmallShadowRandomSelectChance() const
{
    return SmallShadow_RandomSelectChance;
}

void UEnemySettings::SetSmallShadowRandomSelectChance(float NewValue)
{
    SmallShadow_RandomSelectChance = NewValue;
    OnEnemySettingChanged.Broadcast("SmallShadow_RandomSelectChance", NewValue);
}