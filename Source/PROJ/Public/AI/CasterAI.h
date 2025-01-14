// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Arena/Unit/UnitBase.h"
#include "Arena/ArenaManager.h"
#include "CasterAI.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API ACasterAI : public AUnitBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	virtual bool ActionTurn() override;
	UFUNCTION(BlueprintCallable)
	virtual void OnDeath() override;

	UFUNCTION(BlueprintCallable, Category="FighterAI")
	void SetXPReward(const int XP);
	UFUNCTION(BlueprintCallable, Category="FighterAI")
	int GetXPReward();
	
	TArray<AHeroUnit*> HeroList;	// List containing heroes from ArenaManager so the enemy can randomly select one from it.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FighterAI")
	int MaxHP = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FighterAI")
	int Health = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FighterAI")
	int Mana = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FighterAI")
	int Initiative = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FighterAI")
	int Damage = 10;
	
	
private:
	AHeroUnit* RandomlySelectTarget();

	AHeroUnit* TargetHero;

	UPROPERTY(VisibleAnywhere)
	UArenaManager* ArenaManager;

	int XPReward = 0;
};
