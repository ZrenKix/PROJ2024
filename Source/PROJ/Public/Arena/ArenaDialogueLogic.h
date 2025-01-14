// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DialougeDataAsset.h"
#include "Containers/Queue.h"
#include "Subsystems/WorldSubsystem.h"
#include "ArenaDialogueLogic.generated.h"

class UArenaDialogueWidget;
class UArenaManager;

UCLASS()
class PROJ_API UArenaDialogueLogic : public UObject
{
	GENERATED_BODY()

public:
	UArenaDialogueLogic();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void InitializDialogue(TSubclassOf<UDialogueDataAsset> DialogueDataAssetClass, UArenaDialogueWidget* Widget);

	UFUNCTION(BlueprintCallable, Category="Dialogue System")
	void CheckAndDisplayEntries();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	UDialogueDataAsset* DialougeData;

	UPROPERTY()
	UArenaManager* ArenaManager;
	
private:
	void DisplayDialogue(FDialogueCondition* Entry);
	void FinishedDisplaying();

	UPROPERTY()
	bool bIsDisplaying = false;
	UPROPERTY()
	TArray<FDialogueCondition> EntryQueue;
	
	UPROPERTY()
	UArenaDialogueWidget* WidgetInstance;
};