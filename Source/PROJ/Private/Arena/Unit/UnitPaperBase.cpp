// © 2024 Choklad4Life. All rights reserved.


#include "Arena/Unit/UnitPaperBase.h"

#include "Components/DecalComponent.h"
#include "PROJ/PROJ.h"
#include "UObject/ConstructorHelpers.h"

AUnitPaperBase::AUnitPaperBase()
{
	// Initialize the Sprite Component
	Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
	Sprite->SetupAttachment(RootComponent);

	// Initialize the Animation Component
	AnimationComponent = CreateDefaultSubobject<UPaperZDAnimationComponent>(TEXT("AnimationComponent"));

	// Set the AnimInstanceClass (assign your AnimInstance Blueprint class here)
	static ConstructorHelpers::FClassFinder<UPaperZDAnimInstance> AnimBPClass(TEXT("/Game/PathToYourAnimBP"));
	if (AnimBPClass.Succeeded())
	{
		AnimationComponent->SetAnimInstanceClass(AnimBPClass.Class);
	}
}

void AUnitPaperBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (AnimationComponent)
	{
		// Assign the Sprite to the AnimationComponent's render component if needed
		// Some versions of PaperZD automatically detect the Sprite component

		// Obtain the AnimInstance from the AnimationComponent
		AnimInstance = AnimationComponent->GetAnimInstance();
	}
}

TSubclassOf<UPaperZDAnimInstance> AUnitPaperBase::GetSequencerAnimInstanceClass() const
{
	return AnimationComponent ? AnimationComponent->GetAnimInstanceClass() : nullptr;
}

UPaperZDAnimInstance* AUnitPaperBase::GetSequencerAnimInstance()
{
	return AnimInstance;
}

UPaperZDAnimInstance* AUnitPaperBase::GetAnimInstance() const
{
	return AnimInstance;
}

void AUnitPaperBase::OnTargeted_Implementation()
{
	if(TargetDecal)
	{
		TargetDecal->SetActive(true);
		UE_LOG(LogTemp, Display, TEXT("OnTargeted"));
	}
}

void AUnitPaperBase::OnTargetedEnd_Implementation()
{
	if(TargetDecal)
	{
		TargetDecal->SetActive(false);
	}
}