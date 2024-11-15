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

	/** TILLFÄLLIGT */

	// Health property
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float Health;

	// Function to determine if the enemy is alive
	UFUNCTION(BlueprintCallable, Category="Stats")
	bool IsAlive() const;

	void Attack(class ABaseCharacter* Target);
	
	void Die();
	void NotifyPlayerOfDeath();

	/** TILLFÄLLIGT SLUT */
	
	/** ITargetInterface */
	// Override interface functions
	virtual void OnTargeted_Implementation() override;
	virtual void OnTargetedEnd_Implementation() override;
	/** End ITargetInterface */
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
};
