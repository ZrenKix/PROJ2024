// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CombatAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API ACombatAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	ACombatAIController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	UBlackboardComponent* BlackboardComponent;

	void RunBehaviourTree();
};