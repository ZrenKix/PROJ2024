// © 2024 Choklad4Life. All rights reserved.


#include "Character/CompanionCharacter.h"
#include "Character/CompanionAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/MeshComponent.h"
#include "AIController.h"   
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ACompanionCharacter::ACompanionCharacter() : followDistanceThreshold(50.0f), OffsetDistance(50.0f), CompanionType(ECompanionType::Vincent)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	AIControllerClass = ACompanionAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ACompanionCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	
	FollowTargetPosition = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void ACompanionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bCanFollow)
	{
		FollowTargetActor(FollowTargetPosition);
	}
	if (bIsBeingPushed)
	{
		UpdatePush(DeltaTime);
	}
	if (CompanionType == ECompanionType::Vincent)
	{
		if (bIsComforted)
		{
			GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
			bCanFollow = true;
		}
		else
		{
			float DistanceToEnemy = GetClosestEnemyDistance();
			float MinSpeed = 0.0f;
			float DangerRadius = 500.0f; 
			//UE_LOG(LogTemp, Warning, TEXT("DistanceToEnemy: %f"), DistanceToEnemy);

			if (DistanceToEnemy > DangerRadius || DistanceToEnemy < 0.0f)
			{
				GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
				bCanFollow = true;
			}
			else if (DistanceToEnemy <= 150.0f)
			{
				bCanFollow = false;
				GetCharacterMovement()->MaxWalkSpeed = 0.0f;
			}
			else
			{
				float Alpha = FMath::Clamp(DistanceToEnemy / DangerRadius, 0.0f, 1.0f);
				GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(MinSpeed, MovementSpeed, Alpha);
				bCanFollow = true;
			}
		}
	}
}

float ACompanionCharacter::GetClosestEnemyDistance()
{
	float ClosestDistance = -1.0f;
	TArray<AActor*> OverlappedActors;
	float DetectionRadius = 500.0f;

	//DrawDebugSphere(GetWorld(), GetActorLocation(), DetectionRadius, 16, FColor::Red, false, 0.5f);

	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		DetectionRadius,
		{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) },
		AActor::StaticClass(),
		{},
		OverlappedActors
	);

	if (!bHit || OverlappedActors.Num() == 0)
	{
		return -1.0f;
	}

	for (AActor* FoundActor : OverlappedActors)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Overlapping n actors: %d"), OverlappedActors.Num());
		//UE_LOG(LogTemp, Warning, TEXT("Actor name: %s"), *FoundActor->GetName());
		if (FoundActor && FoundActor->ActorHasTag("shadowenemy"))
		{
			UE_LOG(LogTemp, Warning, TEXT("Found shadowenemy"));
			float Distance = (FoundActor->GetActorLocation() - GetActorLocation()).Size();
			if (ClosestDistance < 0.0f || Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
			}
		}
	}

	return ClosestDistance;
}

void ACompanionCharacter::Yeet()
{
	UE_LOG(LogTemp, Warning, TEXT("Companion har blivit puttad!"));
	
	if (FollowTargetPosition)
	{
		FVector Direction = (GetActorLocation() - FollowTargetPosition->GetActorLocation()).GetSafeNormal();
		PushStartLocation = GetActorLocation();
		PushTargetLocation = PushStartLocation + Direction * 200.0f; 
		PushElapsedTime = 0.0f;
		bIsBeingPushed = true;
	}
	
}

void ACompanionCharacter::UpdatePush(float DeltaTime)
{
	PushElapsedTime += DeltaTime;

	if (PushElapsedTime < PushDuration)
	{
		FVector NewLocation = FMath::Lerp(PushStartLocation, PushTargetLocation, PushElapsedTime / PushDuration);
		SetActorLocation(NewLocation, true);
	}
	else
	{
		SetActorLocation(PushTargetLocation, true);
		bIsBeingPushed = false;
	}
}

void ACompanionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACompanionCharacter::FollowTargetActor(AActor* Actor)
{
    if (!Actor) 
    {
        UE_LOG(LogTemp, Warning, TEXT("FollowTargetActor: Actor är null!"));
        return; 
    }

    SetFollowTarget(Actor);

    if (FollowTargetPosition)
    {
        if (AIController)
        {
            FVector Offset(-OffsetDistance, 0.0f, 0.0f); 
            FVector DesiredPosition = FollowTargetPosition->GetActorLocation() + FollowTargetPosition->GetActorRotation().RotateVector(Offset);

            float DistanceToPlayer = FVector::Dist(GetActorLocation(), FollowTargetPosition->GetActorLocation());
            float DistanceToDesiredPosition = FVector::Dist(GetActorLocation(), DesiredPosition);

            //UE_LOG(LogTemp, Warning, TEXT("Companion: Current Location: %s"), *GetActorLocation().ToString());
            //UE_LOG(LogTemp, Warning, TEXT("Companion: DesiredPosition: %s, DistanceToDesiredPosition: %.2f"), *DesiredPosition.ToString(), DistanceToDesiredPosition);

            if (DistanceToDesiredPosition > followDistanceThreshold)
            {
                //UE_LOG(LogTemp, Warning, TEXT("Companion: För långt från DesiredPosition, försöker flytta."));
                EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(DesiredPosition);

                if (MoveResult == EPathFollowingRequestResult::Failed)
                {
                   // UE_LOG(LogTemp, Error, TEXT("Companion: MoveToLocation misslyckades till DesiredPosition!"));
                }
            }
            else
            {
                //UE_LOG(LogTemp, Warning, TEXT("Companion: Nära nog till DesiredPosition, stannar."));
                AIController->StopMovement();
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Companion: AIController är null!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Companion: FollowTargetPosition är null!"));
    }
}

void ACompanionCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AIController = Cast<ACompanionAIController>(NewController);

	if (AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController tilldelad i PossessedBy."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController är null i PossessedBy."));
	}
}

void ACompanionCharacter::SetFollowTarget(AActor* Target)
{
	FollowTargetPosition = Target;
}

void ACompanionCharacter::SetOffsetDistance(float Distance)
{
	OffsetDistance = Distance;
}



