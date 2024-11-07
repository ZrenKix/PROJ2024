// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ProjCharacterBase.h"
#include "Interaction/TargetInterface.h"
#include "ProjEnemy.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API AProjEnemy : public AProjCharacterBase, public ITargetInterface
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
};
