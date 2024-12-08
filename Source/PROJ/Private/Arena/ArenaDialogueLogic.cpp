// © 2024 Choklad4Life. All rights reserved.

#include "Arena/ArenaDialogueLogic.h"

#include "Arena/ArenaManager.h"
#include "Arena/Unit/UnitBase.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

void UArenaDialogueLogic::InitializeDialogue(UDialougeDataAsset* DataAsset, UWorld* World)
{
    DialougeData = DataAsset;
    
    if (World)
    {
        ArenaManager = World->GetSubsystem<UArenaManager>();
        if (!ArenaManager)
        {
            UE_LOG(LogTemp, Warning, TEXT("ArenaManager not found in ArenaDialogueLogic!"));
        }
    }
}

void UArenaDialogueLogic::CheckAndDisplayEntries()
{
    if (!DialougeData || !ArenaManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueData or ArenaManager is not initialized."));
        return;
    }

    for (const FDialogueCondition& Entry : DialougeData->DialogueEntries)
    {
        switch (Entry.TriggerType)
        {
        case EDialogueTrigger::OnSpecificTurn:
            if (Entry.SpecificTurn == ArenaManager->GetCurrentRound())
            {
                DisplayDialogue(Entry.DialogueText);
            }
            break;

        case EDialogueTrigger::OnSpecificHP:
            if (Entry.Unit)
            {
                if (Entry.Unit->CurrentHealth <= Entry.HPThreshold)
                {
                    DisplayDialogue(Entry.DialogueText);
                }
            }
            break;

        default:
            break;
        }
    }
}

void UArenaDialogueLogic::DisplayDialogue(const FText& Dialogue)
{
    if (WidgetTemplate)
    {
        if (!WidgetInstance)
        {
            UWorld* World = GEngine->GetWorldFromContextObjectChecked(ArenaManager);
            if (World)
            {
                APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
                if (PlayerController)
                {
                    WidgetInstance = CreateWidget<UUserWidget>(PlayerController, WidgetTemplate);
                }
            }
        }

        if (WidgetInstance)
        {
            if (!WidgetInstance->IsInViewport())
            {
                WidgetInstance->AddToViewport();
            }

            // Display widget
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("WidgetTemplate is not set in ArenaDialogueLogic."));
    }
}