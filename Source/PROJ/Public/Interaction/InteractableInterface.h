// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJ_API IInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "InteractableInterface", meta = (ReturnDisplayName = "DelayInteractEnabled?"))
	bool Interact();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "InteractableInterface")
	void GetInteractPrompt(FText &InteractText);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "InteractableInterface")
	void SetInteractable(bool bIsInteractable);
};
