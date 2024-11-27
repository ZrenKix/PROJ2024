// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ProjAttributeSet.h"
#include "Arena/Unit/UnitBase.h"
#include "HeroUnit.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API AHeroUnit : public AUnitBase
{
	GENERATED_BODY()

public:
	AHeroUnit();

private:
	virtual void InitAbilityActorInfo() override;
	

public:
	bool IsDead() const;
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual bool ActionTurn() override;
	virtual int GainXp(int Amount) override;
	virtual bool LevelUp() override;
	virtual void OnDeath() override;

	virtual int32 GetPlayerLevel() override;

private:
	int Xp;
	int RequiredXp;
};
