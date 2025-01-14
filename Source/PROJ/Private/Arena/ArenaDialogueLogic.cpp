// © 2024 Choklad4Life. All rights reserved.

#include "Arena/ArenaDialogueLogic.h"

#include "AbilitySystem/ProjAttributeSet.h"
#include "Arena/ArenaDialogueWidget.h"
#include "Arena/ArenaManager.h"
#include "Arena/Unit/UnitBase.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

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
        WidgetInstance->OnDialogueFinished.AddUObject(this, &UArenaDialogueLogic::FinishedDisplaying);
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
    bool DidntFindAUnit = false;
    for (FDialogueCondition& Entry : DialougeData->DialogueEntries)
    {
        if (!Entry.Unit && !Entry.UnitName.IsNone())
        {
            for (AUnitBase* Unit : ArenaManager->GetUnitQueue())
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
                DidntFindAUnit = true;
            }
        }
    }

    if (DidntFindAUnit)
    {
        UE_LOG(LogTemp, Display, TEXT("Trying to find units outside of arena for failed entries."));
        TArray<AActor*> FoundUnits;
        UGameplayStatics::GetAllActorsOfClass(ArenaManager->GetWorld(), AUnitBase::StaticClass(), FoundUnits);
        UE_LOG(LogTemp, Display, TEXT("Found %d units in world."), FoundUnits.Num());
        for (FDialogueCondition& Entry : DialougeData->DialogueEntries)
        {
            if (!Entry.Unit)
            {
                for (AActor* Unit : FoundUnits)
                {
                    AUnitBase* UnitBase = Cast<AUnitBase>(Unit);
                    if (Entry.UnitName == UnitBase->UnitName)
                    {
                        Entry.Unit = Cast<AUnitBase>(Unit);
                        UE_LOG(LogTemp, Display, TEXT("Found %s"), *Entry.UnitName.ToString());
                        break;
                    }
                }
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
    
    for (int32 i = 0; i < DialougeData->DialogueEntries.Num(); ++i)
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
                float CurrentHP = Cast<UProjAttributeSet>(Entry.Unit->GetAttributeSet())->Health.GetCurrentValue();
                if (CurrentHP <= CurrentHP * Entry.HPThreshold)
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
            --i;
        }
    }
}

void UArenaDialogueLogic::DisplayDialogue(FDialogueCondition* Entry)
{
    if (Entry && !EntryQueue.Contains(*Entry))
    {
        UE_LOG(LogTemp, Log, TEXT("Adding dialogue entry to queue: %s"), *Entry->UnitName.ToString());
        EntryQueue.Add(*Entry);
    }

    if (WidgetInstance && Entry && !bIsDisplaying)
    {
        UE_LOG(LogTemp, Log, TEXT("Displaying dialogue: %s"), *Entry->UnitName.ToString());
        
        bIsDisplaying = true;
        WidgetInstance->DisplayDialogue(*Entry);
    }
    else
    {
        if (bIsDisplaying)
        {
            UE_LOG(LogTemp, Warning, TEXT("Attempted to display dialogue, but another dialogue is already being displayed"));
        }
    }
}

void UArenaDialogueLogic::FinishedDisplaying()
{
    bIsDisplaying = false;
    
    if (!EntryQueue.IsEmpty())
    {
        UE_LOG(LogTemp, Log, TEXT("Dialogue finished. Queue size before removing entry: %d"), EntryQueue.Num());
        
        EntryQueue.RemoveAt(0);
        
        UE_LOG(LogTemp, Log, TEXT("Queue size after removing entry: %d"), EntryQueue.Num());

        if (!EntryQueue.IsEmpty())
        {
            UE_LOG(LogTemp, Log, TEXT("Displaying next dialogue: %s"), *EntryQueue[0].UnitName.ToString());
            DisplayDialogue(&EntryQueue[0]);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("No more dialogues in the queue. Finished displaying all dialogues."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("FinishedDisplaying called, but the queue is empty"));
    }
}
