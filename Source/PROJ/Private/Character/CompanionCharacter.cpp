// © 2024 Choklad4Life. All rights reserved.


#include "Character/CompanionCharacter.h"

// Sets default values
ACompanionCharacter::ACompanionCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACompanionCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACompanionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACompanionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

