// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ProjPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/ProjAbilitySystemComponent.h"
#include "Input/DBInputComponent.h"
#include "Interaction/PlayerInterface.h"
#include "Interaction/TargetInterface.h"

AProjPlayerController::AProjPlayerController()
{
	bReplicates = true;
}

void AProjPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	CursorTrace();
}

void AProjPlayerController::CursorTrace()
{
    // Perform the cursor trace
    GetHitResultUnderCursor(ECC_Visibility, false, CursorHitResult);

    // Get the hit actor
    AActor* HitActor = CursorHitResult.GetActor();

    // If nothing is hit, clear the current target
    if (!CursorHitResult.bBlockingHit)
    {
        if (ThisTarget)
        {
            // End targeting for the previous target if it implemented any interface
            if (ThisTarget->GetClass()->ImplementsInterface(UPlayerInterface::StaticClass()))
            {
                IPlayerInterface::Execute_OnPlayerTargetedEnd(ThisTarget);
            }
            if (ThisTarget->GetClass()->ImplementsInterface(UTargetInterface::StaticClass()))
            {
                ITargetInterface::Execute_OnTargetedEnd(ThisTarget);
            }
        }

        ThisTarget = nullptr;
        return;
    }

    // If the hit actor is different from the current target
    if (HitActor != ThisTarget)
    {
        // End targeting for the previous target if it implemented any interface
        if (ThisTarget)
        {
            if (ThisTarget->GetClass()->ImplementsInterface(UPlayerInterface::StaticClass()))
            {
                IPlayerInterface::Execute_OnPlayerTargetedEnd(ThisTarget);
            }
            if (ThisTarget->GetClass()->ImplementsInterface(UTargetInterface::StaticClass()))
            {
                ITargetInterface::Execute_OnTargetedEnd(ThisTarget);
            }
        }

        // Update the current target
        ThisTarget = HitActor;

        // Start targeting for the new target if it implements any interface
        if (ThisTarget)
        {
            if (ThisTarget->GetClass()->ImplementsInterface(UPlayerInterface::StaticClass()))
            {
                IPlayerInterface::Execute_OnPlayerTargeted(ThisTarget);
            }
            if (ThisTarget->GetClass()->ImplementsInterface(UTargetInterface::StaticClass()))
            {
                ITargetInterface::Execute_OnTargeted(ThisTarget);
            }
        }
    }
}


void AProjPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
	if(GetAbilitySystemComponent() == nullptr) return;
	GetAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
}

void AProjPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(GetAbilitySystemComponent() == nullptr) return;
	GetAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
}

void AProjPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if(GetAbilitySystemComponent() == nullptr) return;
	GetAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
}

UProjAbilitySystemComponent* AProjPlayerController::GetAbilitySystemComponent()
{
	if(AbilitySystemComponent == nullptr)
	{
		AbilitySystemComponent = Cast<UProjAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AbilitySystemComponent;
}

void AProjPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Stoppar programmet om ProjContext Ã¤r nullptr
	check(ProjContext);

	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(LocalPlayerSubsystem)
	{
		LocalPlayerSubsystem->AddMappingContext(ProjContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AProjPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UDBInputComponent* DBInputComponent = CastChecked<UDBInputComponent>(InputComponent);
	DBInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProjPlayerController::Move);
	DBInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AProjPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}