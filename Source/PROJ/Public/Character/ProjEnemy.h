// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arena/Unit/UnitBase.h"
#include "Interaction/TargetInterface.h"
#include "ProjEnemy.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API AProjEnemy : public AUnitBase, public ITargetInterface
{
	GENERATED_BODY()
	AProjEnemy();

public:
	
	/** ITargetInterface */
	virtual void OnTargeted() override;
	virtual void OnTargetedEnd() override;
	/** End ITargetInterface */
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
};
