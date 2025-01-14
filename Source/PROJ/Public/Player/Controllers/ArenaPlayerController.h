#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Player/ProjPlayerController.h"
#include "ArenaPlayerController.generated.h"

class UDamageTextComponent;
class AHeroUnit;
class UInputMappingContext;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityInputExecuted, AUnitBase*, Unit);

UCLASS()
class PROJ_API AArenaPlayerController : public AProjPlayerController
{
	GENERATED_BODY()

public:
	AArenaPlayerController();

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float Damage, ACharacter* TargetCharacter);

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnAbilityInputExecuted OnAbilityInputExecuted;

	UFUNCTION()
	void PossessHero(AHeroUnit* Hero);

	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	void OnActionEnd();
	
protected:
	virtual void BeginPlay() override;
	
	void ResetCastingState();

private:	
	UPROPERTY()
	AHeroUnit* CurrentHero;

	void SetupInputMapping();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
};