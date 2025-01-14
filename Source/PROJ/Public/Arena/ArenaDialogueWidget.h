// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArenaDialogueWidget.generated.h"

struct FDialogueCondition;

DECLARE_MULTICAST_DELEGATE(FOnDialogueDisplayed);

UCLASS()
class PROJ_API UArenaDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnDialogueDisplayed OnDialogueFinished;
	
	UFUNCTION(BlueprintCallable)
	void FinishDisplayingDialogue();
	
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayDialogue(FDialogueCondition Entry);
};