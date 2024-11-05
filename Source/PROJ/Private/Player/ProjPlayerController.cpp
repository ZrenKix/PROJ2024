// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ProjPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
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
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	if(!HitResult.bBlockingHit) return;

	LastTarget = ThisTarget;
	ThisTarget = HitResult.GetActor();

	if(LastTarget == nullptr)
	{
		if(ThisTarget != nullptr)
		{
			ThisTarget->OnTargeted();
		}
	}
	else // LastTarget is valid
	{
		if(ThisTarget == nullptr)
		{
			LastTarget->OnTargetedEnd();
		}
		else // Both targets are valid
		{
			if(LastTarget != ThisTarget)
			{
				// End the last target and start the new one
				LastTarget->OnTargetedEnd();
				ThisTarget->OnTargeted();
			}
		}
	}
	
}

void AProjPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Stoppar programmet om ProjContext är nullptr
	check(ProjContext);

	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(LocalPlayerSubsystem);
	LocalPlayerSubsystem->AddMappingContext(ProjContext, 0);

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

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProjPlayerController::Move);
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


