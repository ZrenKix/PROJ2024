// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ProjAttributeSet.h"
#include "Arena/Unit/UnitBase.h"
#include "HeroUnit.generated.h"

UCLASS()
class PROJ_API AHeroUnit : public AUnitBase
{
	GENERATED_BODY()

public:
	AHeroUnit();
	
public:
	

	virtual void InitAbilityActorInfo() override;
	
	bool IsDead() const
	{
		if (AttributeSet)
			return Cast<UProjAttributeSet>(AttributeSet)->GetHealth() <= 0;
		return false;
	}
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_PlayerState() override;

	virtual bool ActionTurn() override;
	void OnActionTurnEnd();
	virtual int GainXp(int Amount) override;
	virtual bool LevelUp() override;
	virtual void OnDeath() override;

	virtual int32 GetPlayerLevel() override;

protected:
	virtual void BeginPlay() override;
	
private:
	int Xp;
	int RequiredXp;
};
