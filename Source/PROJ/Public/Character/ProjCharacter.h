// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arena/Unit/UnitBase.h"
#include "ProjCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API AProjCharacter : public AUnitBase
{
	GENERATED_BODY()

public:
	AProjCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	void InitAbilityActorInfo();
};
