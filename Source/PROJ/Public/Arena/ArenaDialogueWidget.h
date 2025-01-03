// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArenaDialogueWidget.generated.h"

struct FDialogueCondition;

UCLASS()
class PROJ_API UArenaDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayDialogue(FDialogueCondition Entry);
};
