// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CompanionAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API ACompanionAIController : public AAIController
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	void MoveToTarget(AActor* TargetActor, float AcceptanceRadius = 50.0f);

};
