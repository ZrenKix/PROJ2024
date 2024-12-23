// © 2024 Choklad4Life. All rights reserved.


#include "AI/CombatAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ACombatAIController::ACombatAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	
}

void ACombatAIController::BeginPlay()
{
	Super::BeginPlay();

	if(BehaviorTree)
	{
		if(UseBlackboard(BehaviorTree->BlackboardAsset, BlackboardComponent))
		{
			RunBehaviorTree(BehaviorTree);
		}
	}
}

void ACombatAIController::RunBehaviourTree()
{
	if (BehaviorTree && RunBehaviorTree(BehaviorTree))
	{
		UE_LOG(LogTemp, Warning, TEXT("Behavior Tree started successfully!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to start Behavior Tree"));
	}
}
