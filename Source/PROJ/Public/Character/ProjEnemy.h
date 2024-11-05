// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ProjCharacter.h"
#include "Interaction/TargetInterface.h"
#include "ProjEnemy.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API AProjEnemy : public AProjCharacter, public ITargetInterface
{
	GENERATED_BODY()
	AProjEnemy();

public:
	virtual void OnTargeted() override;
	virtual void OnTargetedEnd() override;
};
