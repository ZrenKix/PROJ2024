// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ProjPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/ProjAbilitySystemComponent.h"
#include "Input/DBInputComponent.h"
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
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHitResult);

	if(!CursorHitResult.bBlockingHit) return;

	LastTarget = ThisTarget;
	ThisTarget = CursorHitResult.GetActor();

	if(LastTarget != ThisTarget)
	{
		if(LastTarget) LastTarget->OnTargetedEnd();
		if(ThisTarget) ThisTarget->OnTargeted();
	}
	
}

void AProjPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
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

	// Stoppar programmet om ProjContext är nullptr
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

	UE_LOG(LogTemp, Warning, TEXT("Mapping in super"));

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


