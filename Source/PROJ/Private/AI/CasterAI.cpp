// © 2024 Choklad4Life. All rights reserved.


#include "AI/CasterAI.h"

void ACasterAI::BeginPlay()
{
	// Get HeroList from ArenaManager
	//HeroList = ArenaManager->GetHeroes();
	ArenaManager = GetWorld()->GetSubsystem<UArenaManager>();
	HeroList = ArenaManager->GetHeroes();
	
	ActionTurn(); // For testing 
}

void ACasterAI::OnDeath()
{
	// Play death effects or animations here. ArenaManager handles the actual death i think.
	UE_LOG(LogTemp, Warning, TEXT("I Died!"));
}

bool ACasterAI::ActionTurn()
{
	// Randomly selects a hero from the herolist and sets it to Target.
	TargetHero = RandomlySelectTarget();

	// Attack target.
	UE_LOG(LogTemp, Warning, TEXT("Deal damage to target hero"));
	
	return true;
}


AHeroUnit* ACasterAI::RandomlySelectTarget()
{
	// Check that HeroList is not empty
	if(!HeroList.IsEmpty())
	{
		
		// Generate a random number between 0 and 2. This is because the hero party will contain 3 targets max.
		int RandomInt = FMath::RandRange(0, HeroList.Num() - 1);
		
		//UE_LOG(LogTemp, Display, TEXT("Selected hero: %s"), HeroList[RandomInt]);
			
		// Returns a randomly selected hero from HeroList.
		return HeroList[RandomInt];
		
	}
	// If HeroList is empty.
	UE_LOG(LogTemp, Warning, TEXT("HeroList is empty!"));
	return nullptr;
}

void ACasterAI::SetXPReward(const int XP)
{
	XPReward = XP;
}

int ACasterAI::GetXPReward()
{
	return XPReward;
}