// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJ_API ITargetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Function called when the actor is targeted
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Target")
	void OnTargeted();

	// Function called when the actor is no longer targeted
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Target")
	void OnTargetedEnd();
};
