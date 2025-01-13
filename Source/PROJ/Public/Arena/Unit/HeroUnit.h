// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Arena/Unit/UnitBase.h"
#include "Interaction/PlayerInterface.h"
#include "HeroUnit.generated.h"

class UNiagaraComponent;

UCLASS()
class PROJ_API AHeroUnit : public AUnitBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AHeroUnit();

private:
	virtual void InitAbilityActorInfo() override;
	

public:
	UFUNCTION(BlueprintCallable)
	bool IsDead() const;
	
	virtual void BeginPlay() override;
	virtual bool ActionTurn() override;
	virtual int GainXp(int Amount) override;
	virtual bool LevelUp() override;
	virtual void OnDeath() override;

	virtual void OnPlayerTargeted_Implementation() override;
	virtual void OnPlayerTargetedEnd_Implementation() override;

	UFUNCTION(BlueprintCallable)
	void RemoveDMGBuff();

	UFUNCTION(BlueprintCallable)
	void StartDMGBuffEffect();

	UPROPERTY(EditAnywhere, Category="Combat")
	UNiagaraComponent* DMGBuffEffect;

	virtual int32 GetPlayerLevel() override;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;
	virtual FVector GetCombatSocketLocation() override;

	UFUNCTION(BlueprintCallable)
	void ToggleEnemyWidget();

	UFUNCTION(BlueprintCallable)
	bool HasAttackedThisTurn() const
	{
		return bHasAttackedThisTurn;
	}

	UFUNCTION(BlueprintCallable)
	void SetHasAttackedThisTurn(bool HasAttackedThisTurn)
	{
		this->bHasAttackedThisTurn = HasAttackedThisTurn;
	}
	
	
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> EnemySettingsWidgetClass;
	
	
	UPROPERTY()
	UUserWidget* EnemySettingsWidget;
private:
	bool bIsSettingsMenuOpen = false; 
	int Xp;
	int RequiredXp;
	
	bool bHasAttackedThisTurn = false;
};
