// © 2024 Choklad4Life. All rights reserved.


#include "Arena/ArenaDialogueWidget.h"

void UArenaDialogueWidget::FinishDisplayingDialogue()
{
	OnDialogueFinished.Broadcast();
}