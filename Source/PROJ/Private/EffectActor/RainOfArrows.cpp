// © 2024 Choklad4Life. All rights reserved.


#include "EffectActor/RainOfArrows.h"

#include "Components/DecalComponent.h"

// Sets default values
ARainOfArrows::ARainOfArrows()
{
	PrimaryActorTick.bCanEverTick = true;

	ArrowCircleDecal = CreateDefaultSubobject<UDecalComponent>("ArrowCircleDecal");
	RootComponent = ArrowCircleDecal;
}
void ARainOfArrows::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARainOfArrows::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

