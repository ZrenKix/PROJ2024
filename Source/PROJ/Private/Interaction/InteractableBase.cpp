// © 2024 Choklad4Life. All rights reserved.


#include "Interaction/InteractableBase.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values
AInteractableBase::AInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create components and attach to root.
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	// WidgetComponent setup (for showing interact)
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(Root);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/Blueprints/UI/Interaction/WBP_InteractPrompt")); // Finds WBP_InteractPrompt to set it as default.
	if(WidgetClassFinder.Succeeded())
	{
		if (WidgetComponent)
		{
			WidgetComponent->SetWidgetClass(WidgetClassFinder.Class);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Widget Blueprint not found!"));
		}
	}
	// Static mesh component setup (for static objects)
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(Root);
	StaticMeshComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
	// Skeletal mesh component setup (For actors that need animations, NPC etc.)
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(Root);
	SkeletalMeshComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
}

// Called when the game starts or when spawned
void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	// Hide widget component
	WidgetComponent->SetVisibility(false);
	
	// Get player reference
	GetPlayer();
	
}

void AInteractableBase::GetPlayer()
{
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player character found: %s"), *Player->GetName());
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No player character found!"));
	}
}






