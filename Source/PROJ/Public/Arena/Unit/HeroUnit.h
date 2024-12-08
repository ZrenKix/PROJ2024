// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Arena/Unit/UnitBase.h"
#include "HeroUnit.generated.h"

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
	
	virtual void BeginPlay() override;
	virtual bool ActionTurn() override;
	virtual int GainXp(int Amount) override;
	virtual bool LevelUp() override;
	virtual void OnDeath() override;

	virtual int32 GetPlayerLevel() override;

private:
	int Xp;
	int RequiredXp;
};
