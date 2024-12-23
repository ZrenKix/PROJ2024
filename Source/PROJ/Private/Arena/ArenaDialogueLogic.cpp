// © 2024 Choklad4Life. All rights reserved.

#include "Arena/ArenaDialogueLogic.h"

#include "AbilitySystem/ProjAttributeSet.h"
#include "Arena/ArenaDialogueWidget.h"
#include "Arena/ArenaManager.h"
#include "Arena/Unit/UnitBase.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

UArenaDialogueLogic::UArenaDialogueLogic()
    : DialougeData(nullptr),
      ArenaManager(nullptr),
      WidgetInstance(nullptr)
{
}

void UArenaDialogueLogic::InitializDialogue(TSubclassOf<UDialogueDataAsset> DialogueDataAssetClass, UArenaDialogueWidget* Widget)
{
    // Ensure we have a valid class reference
    if (!DialogueDataAssetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid Dialogue Data Asset class."));
        return;
    }

    // Spawn or instantiate the DialogueDataAsset class
    DialougeData = NewObject<UDialogueDataAsset>(this, DialogueDataAssetClass); 

    // Check if we successfully created the object
    if (!DialougeData)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to instantiate DialogueDataAsset."));
        return;
    }

    WidgetInstance = Widget;

    if (WidgetInstance)
    {
        UE_LOG(LogTemp, Log, TEXT("Widget successfully assigned to DialogueLogic."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("WidgetInstance is null in InitializeDialogue."));
    }

    if (!IsValid(DialougeData))
    {
        UE_LOG(LogTemp, Error, TEXT("DialogueData is empty."));
        return;
    }

    // Initialize the dialogue entries from the asset
    for (FDialogueCondition& Entry : DialougeData->DialogueEntries)
    {
        if (!Entry.Unit && !Entry.UnitName.IsNone())
        {
            // Assuming you have a function to get units from ArenaManager
            for (AUnitBase* Unit : ArenaManager->GetUnits())
            {
                if (Unit && Unit->UnitName == Entry.UnitName)
                {
                    Entry.Unit = Unit;
                    break;
                }
            }

            if (!Entry.Unit)
            {
                UE_LOG(LogTemp, Warning, TEXT("Could not find Unit with name %s"), *Entry.UnitName.ToString());
            }
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
    
    for (int32 i = DialougeData->DialogueEntries.Num() - 1; i >= 0; --i)
    {
        FDialogueCondition& Entry = DialougeData->DialogueEntries[i];
        
        bool bDisplayed = false;
        switch (Entry.TriggerType)
        {
        case EDialogueTrigger::OnSpecificTurn:
            if (Entry.SpecificTurn == ArenaManager->GetCurrentRound())
            {
                DisplayDialogue(&Entry);
                bDisplayed = true;
            }
            break;

        case EDialogueTrigger::OnSpecificHP:
            if (Entry.Unit)
            {
                if (Cast<UProjAttributeSet>(Entry.Unit->GetAttributeSet())->Health.GetCurrentValue() <= Entry.HPThreshold)
                {
                    DisplayDialogue(&Entry);
                    bDisplayed = true;
                }
            }
            break;

        default:
            break;
        }
        
        if (bDisplayed)
        {
            DialougeData->DialogueEntries.RemoveAt(i);
        }
    }
}

void UArenaDialogueLogic::DisplayDialogue(FDialogueCondition* Entry)
{
    if (WidgetInstance && Entry)
    {
        WidgetInstance->DisplayDialogue(*Entry);
    }
}