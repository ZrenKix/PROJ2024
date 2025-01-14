// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DialogueCondition.generated.h"

class AUnitBase;

UENUM(BlueprintType)
enum class EDialogueTrigger : uint8
{
	OnSpecificTurn UMETA(DisplayName = "On Specific Turn"),
	OnSpecificHP UMETA(DisplayName = "On Specific HP Percentage"),	
};

USTRUCT(BlueprintType)
struct FDialogueCondition
{
	GENERATED_BODY()

	// Target unit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	AUnitBase* Unit;

	// Target unit name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FName UnitName = FName();

	// Text to display
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Dialogue")
	FText DialogueText = FText();

	// Trigger type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Dialogue")
	EDialogueTrigger TriggerType = EDialogueTrigger::OnSpecificTurn;
	
	// Trigger type to display on specific turn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Dialogue",meta = (EditCondition = "TriggerType == EDialogueTrigger::OnSpecificTurn"))
	int32 SpecificTurn = 0;

	// Trigger type to display on specific hp threshold
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (EditCondition = "TriggerType == EDialogueTrigger::OnSpecificHP", UIMin = "0.0", UIMax = "1.0"))
	float HPThreshold = 1.0f;

	bool operator==(const FDialogueCondition& Other) const
	{
		return UnitName == Other.UnitName &&
			   DialogueText.EqualTo(Other.DialogueText);
	}
};
