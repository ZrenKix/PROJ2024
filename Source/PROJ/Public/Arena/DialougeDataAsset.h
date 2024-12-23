// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DialogueCondition.h"
#include "DialougeDataAsset.generated.h"

UCLASS(Blueprintable)
class PROJ_API UDialogueDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FDialogueCondition> DialogueEntries;
};