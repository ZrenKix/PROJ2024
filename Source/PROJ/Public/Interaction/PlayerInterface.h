// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJ_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideMagicCircle();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Target")
	void OnPlayerTargeted();

	// Function called when the actor is no longer targeted
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Target")
	void OnPlayerTargetedEnd();
};
