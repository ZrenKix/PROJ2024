#include "Player/Controllers/ArenaPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Input/DBInputConfig.h"
#include "InputMappingContext.h"
#include "AbilitySystem/ProjAbilitySystemComponent.h"
#include "Arena/Unit/HeroUnit.h"
#include "UI/Widget/DamageTextComponent.h"

void AArenaPlayerController::BeginPlay()
{
	SetupInputMapping();
	
	Super::BeginPlay();
}

AArenaPlayerController::AArenaPlayerController(): CurrentHero(nullptr)
{
	// Load the Input Config
	static ConstructorHelpers::FObjectFinder<UDBInputConfig> InputConfigFinder(
		TEXT("DBInputConfig'/Game/Blueprints/Input/DA_DBInputConfig.DA_DBInputConfig'"));
	if (InputConfigFinder.Succeeded())
	{
		InputConfig = InputConfigFinder.Object;
		UE_LOG(LogTemp, Display, TEXT("Successfully loaded InputConfig in AArenaPlayerController."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load InputConfig in AArenaPlayerController."));
	}
	
	// Load the Input Mapping Context
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> ProjContextFinder(
		TEXT("InputMappingContext'/Game/Blueprints/Input/IMC_ProjContext.IMC_ProjContext'"));
	if (ProjContextFinder.Succeeded())
	{
		ProjContext = ProjContextFinder.Object;
		UE_LOG(LogTemp, Display, TEXT("Successfully loaded ProjContext in AArenaPlayerController."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load ProjContext in AArenaPlayerController."));
	}

	// Load the Move Action
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionFinder(
		TEXT("InputAction'/Game/Blueprints/Input/InputActions/IA_Move.IA_Move'"));
	if (MoveActionFinder.Succeeded())
	{
		MoveAction = MoveActionFinder.Object;
		UE_LOG(LogTemp, Display, TEXT("Successfully loaded MoveAction."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load MoveAction."));
	}
}

void AArenaPlayerController::ShowDamageNumber_Implementation(float Damage, ACharacter* TargetCharacter)
{
	if(IsValid(TargetCharacter) && DamageTextComponentClass)
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(Damage);
	}
}

void AArenaPlayerController::OnActionEnd()
{
	UnPossess();
	OnAbilityInputExecuted.Broadcast(CurrentHero);
	UE_LOG(LogTemp, Display, TEXT("Broadcasted OnAbilityInputExecuted for %s"), *GetName());
}

void AArenaPlayerController::SetupInputComponent()
{
	UE_LOG(LogTemp, Warning, TEXT("Setup"));
	Super::SetupInputComponent();
}

void AArenaPlayerController::SetupInputMapping()
{
	if (ProjContext)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(ProjContext, 1); // Priority 1 to avoid conflicts
			UE_LOG(LogTemp, Display, TEXT("ProjContext successfully added in BeginPlay."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to retrieve EnhancedInputLocalPlayerSubsystem."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ProjContext is null. Ensure it is set correctly."));
	}

	UE_LOG(LogTemp, Warning, TEXT("Mapping"));

	Super::SetupInputComponent();
}

void AArenaPlayerController::PossessHero(AHeroUnit* Hero)
{
	if (!Hero)
	{
		UE_LOG(LogTemp, Error, TEXT("PossessHero: Provided HeroUnit is null."));
		return;
	}

	// if (CurrentHero == Hero)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Already possessing hero: %s"), *Hero->GetName());
	// 	return;
	// }

	if (CurrentHero)
		UnPossess();

	CurrentHero = Hero;
	Possess(CurrentHero);

	// Ensure AbilitySystemComponent is set correctly
	if (UProjAbilitySystemComponent* ASC = Hero->FindComponentByClass<UProjAbilitySystemComponent>())
	{
		AbilitySystemComponent = ASC;
		UE_LOG(LogTemp, Display, TEXT("AbilitySystemComponent set for hero: %s"), *Hero->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find AbilitySystemComponent on hero: %s"), *Hero->GetName());
	}

	SetupInputMapping();

	UE_LOG(LogTemp, Display, TEXT("PlayerController now possesses hero: %s"), *GetPawn()->GetName());
}


void AArenaPlayerController::ResetCastingState()
{
	EnableInput(this);
	UE_LOG(LogTemp, Display, TEXT("Casting state reset. Input re-enabled."));
}
