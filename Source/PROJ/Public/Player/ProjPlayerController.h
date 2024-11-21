// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "ProjPlayerController.generated.h"


struct FGameplayTag;
class UDBInputConfig;
class ITargetInterface;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class UProjAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PROJ_API AProjPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AProjPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
protected:
	UPROPERTY(EditAnywhere, Category = "Input Mapping")
	TObjectPtr<UInputMappingContext> ProjContext;

	UPROPERTY(EditAnywhere, Category = "Input Mapping")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();

	// The currently highlighted target
	UPROPERTY()
	AActor* ThisTarget = nullptr;
	//TScriptInterface<ITargetInterface> LastTarget;
	//TScriptInterface<ITargetInterface> ThisTarget;
	FHitResult CursorHitResult;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	virtual void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UDBInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UProjAbilitySystemComponent> AbilitySystemComponent;

	UProjAbilitySystemComponent* GetAbilitySystemComponent();
};
