// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjPlayerController.generated.h"


class ITargetInterface;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
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
private:
	UPROPERTY(EditAnywhere, Category = "Input Mapping")
	TObjectPtr<UInputMappingContext> ProjContext;

	UPROPERTY(EditAnywhere, Category = "Input Mapping")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();

	TScriptInterface<ITargetInterface> LastTarget;
	TScriptInterface<ITargetInterface> ThisTarget;
};
