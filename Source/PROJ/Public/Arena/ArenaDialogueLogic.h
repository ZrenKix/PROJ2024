// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DialougeDataAsset.h"
#include "Subsystems/WorldSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "ArenaDialogueLogic.generated.h"

class UArenaManager;

UCLASS()
class PROJ_API UArenaDialogueLogic : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	UDialougeDataAsset* DialougeData;

public:
	void InitializeDialogue(UDialougeDataAsset* DataAsset, UWorld* World);
	void CheckAndDisplayEntries();

private:
	void DisplayDialogue(const FText& Dialogue);

	UPROPERTY()
	UArenaManager* ArenaManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> WidgetTemplate;

	UPROPERTY()
	UUserWidget* WidgetInstance;
};