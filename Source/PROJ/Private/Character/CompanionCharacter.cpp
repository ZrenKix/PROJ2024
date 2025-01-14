// © 2024 Choklad4Life. All rights reserved.
#include "Character/CompanionCharacter.h"
#include "Character/CompanionAIController.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/MeshComponent.h"
#include "Engine/World.h"

ACompanionCharacter::ACompanionCharacter() 
    : followDistanceThreshold(50.0f)
    , OffsetDistance(50.0f)
    , CompanionType(ECompanionType::Vincent)
    , LastAvoidTime(0.f)                 
    , AvoidCooldown(1.0f)               
{
    PrimaryActorTick.bCanEverTick = true;

    GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
    AIControllerClass = ACompanionAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ACompanionCharacter::BeginPlay()
{
    Super::BeginPlay();

    CachedPlayerRef = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    GetWorldTimerManager().SetTimer(
        DistanceCheckTimerHandle,
        this,
        &ACompanionCharacter::CheckDistancePeriodically,
        DistanceCheckInterval,
        true
    );
}

void ACompanionCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    UpdateVincentFearLogic();
    if (!bCanFollow || !FollowTargetPosition)
    {
        return;
    }
    
    float DistanceToPlayer = FVector::Dist(GetActorLocation(), FollowTargetPosition->GetActorLocation());
    float CurrentTime = GetWorld()->GetTimeSeconds();

    if (bIsSidestepping && (CurrentTime - LastAvoidTime) > AvoidCooldown)
    {
        bIsSidestepping = false;
    }

    bool bTimeForNewSidestep = ((CurrentTime - LastAvoidTime) > AvoidCooldown);

    if (!bIsSidestepping && DistanceToPlayer < MinComfortDistance
    && IsPlayerMovingTowardMe()
    && bTimeForNewSidestep)
    {
        MoveAwayFromPlayer();   
        LastAvoidTime = CurrentTime;
        return;
    }
    if (CachedPlayerRef)
    {
        float PlayerSpeed = CachedPlayerRef->GetVelocity().Size();
        if (PlayerSpeed < 5.f)
        {
            PlayerStillTime += DeltaTime;
        }else
        {
            PlayerStillTime = 0.f;
        }
    }
    if (PlayerStillTime >= RequiredStillTime)
    {
        MoveToSideOfTarget(CachedPlayerRef);
        return;
    }
    if (!bIsSidestepping)
    {
        FollowTargetActor(FollowTargetPosition);
    }
}

void ACompanionCharacter::UpdateVincentFearLogic()
{
    if (CompanionType != ECompanionType::Vincent){ return; }

    if (bIsComforted)
    {
        GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
        bCanFollow = true;
        return;
    }
    
    float DistanceToEnemy = GetClosestEnemyDistance();
    float MinSpeed = 0.0f, DangerRadius = 500.0f;

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
        float Alpha = FMath::Clamp(DistanceToEnemy / DangerRadius, 0.f, 1.f);
        GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(MinSpeed, MovementSpeed, Alpha);
        bCanFollow = true;
    }
}

float ACompanionCharacter::GetClosestEnemyDistance()
{
    float ClosestDistance = -1.0f;
    TArray<AActor*> OverlappedActors;
    float DetectionRadius = 500.0f;

    bool bHit = UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        GetActorLocation(),
        DetectionRadius,
        { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) },
        AActor::StaticClass(),
        {},
        OverlappedActors
    );

    if (!bHit || OverlappedActors.Num() == 0) { return -1.0f; }

    for (AActor* FoundActor : OverlappedActors)
    {
        if (FoundActor && FoundActor->ActorHasTag("shadowenemy"))
        {
            UE_LOG(LogTemp, Warning, TEXT("Found shadowenemy"));
            float Distance = (FoundActor->GetActorLocation() - GetActorLocation()).Size();
            if (ClosestDistance < 0.0f || Distance < ClosestDistance)
                { ClosestDistance = Distance; }
        }
    }
    return ClosestDistance;
}

void ACompanionCharacter::CheckDistancePeriodically()
{
    if (!FollowTargetPosition || !bCanFollow) { return;}
    float CurrentDistanceToPlayer = FVector::Dist(GetActorLocation(), FollowTargetPosition->GetActorLocation());
    if (!bInArena && CurrentDistanceToPlayer > MaxAllowedDistance) { TeleportIfTooFar(); }
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
    if (!FollowTargetPosition) { return; }
    if (!AIController) { return; }
    
    FVector Offset(-OffsetDistance, 0.0f, 0.0f); 
    FVector DesiredPosition = FollowTargetPosition->GetActorLocation() 
        + FollowTargetPosition->GetActorRotation().RotateVector(Offset);

    float DistanceToDesiredPosition = FVector::Dist(GetActorLocation(), DesiredPosition);

    if (DistanceToDesiredPosition > followDistanceThreshold)
    {
        AIController->MoveToLocation(DesiredPosition);
    }
    else if (!bIsSidestepping)
    {
        AIController->StopMovement();
    }
}

void ACompanionCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    AIController = Cast<ACompanionAIController>(NewController);
    if (AIController) {UE_LOG(LogTemp, Warning, TEXT("AIController tilldelad i PossessedBy.")); }
    else { UE_LOG(LogTemp, Warning, TEXT("AIController är null i PossessedBy.")); }
}

void ACompanionCharacter::SetFollowTarget(AActor* Target)
{
    FollowTargetPosition = Target;
}

bool ACompanionCharacter::IsTargetStopped(AActor* Target, float SpeedThreshold) const
{
    ACharacter* Character = Cast<ACharacter>(Target);
    if (!Character) { return false; }
    float Speed = Character->GetVelocity().Size();
    UE_LOG(LogTemp, Warning, TEXT("FollowTargetActor: %f"), Speed);
    return (Speed < SpeedThreshold);
}

void ACompanionCharacter::MoveToSideOfTarget(AActor* Target)
{
    if (!AIController || !Target) { return; }
    FVector SideOffset;
    switch (CompanionType)
    {
    case ECompanionType::Teddy:
        SideOffset = FVector(0.f, -100.f, 0.f);
        break;
    case ECompanionType::Vincent:
        SideOffset = FVector(0.f, 100.f, 0.f);
        break;
    default:
        SideOffset = FVector(-OffsetDistance, 0.f, 0.f);
    }
    FVector NewLocation = Target->GetActorLocation()
        + Target->GetActorRotation().RotateVector(SideOffset);

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSys)
    {
        FNavLocation ProjectedLocation;
        if (NavSys->ProjectPointToNavigation(NewLocation, ProjectedLocation, FVector(100.f, 100.f, 100.f)))
        {
            NewLocation = ProjectedLocation.Location;
        }
    }
    //UE_LOG(LogTemp, Warning, TEXT("Går åt sidan av spelaren"));
    AIController->MoveToLocation(NewLocation);
}

//teleporterar Companion om för långt bort
void ACompanionCharacter::TeleportIfTooFar()
{
    if (!FollowTargetPosition) { return; }

    FVector Offset(-OffsetDistance, 0.f, 0.f);
    FVector DesiredLocation = FollowTargetPosition->GetActorLocation() 
        + FollowTargetPosition->GetActorRotation().RotateVector(Offset);

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys)
    {
        UE_LOG(LogTemp, Warning, TEXT("No NavSys. Teleporting directly."));
        SetActorLocation(DesiredLocation, false, nullptr, ETeleportType::TeleportPhysics);
        return;
    }

    FNavLocation ProjectedLocation;
    FVector QueryExtent(200.f, 200.f, 200.f);
    if (NavSys->ProjectPointToNavigation(DesiredLocation, ProjectedLocation, QueryExtent))
    {
        SetActorLocation(ProjectedLocation.Location, false, nullptr, ETeleportType::TeleportPhysics);
        UE_LOG(LogTemp, Warning, TEXT("finns navmesh här :)"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to project location. Teleporting anyway."));
        SetActorLocation(DesiredLocation, false, nullptr, ETeleportType::TeleportPhysics);
    }
    if (AIController) { AIController->StopMovement(); }
}

void ACompanionCharacter::SetOffsetDistance(float Distance)
{
    OffsetDistance = Distance;
}

bool ACompanionCharacter::IsPlayerMovingTowardMe() const
{
    if (!FollowTargetPosition) { return false; }
    ACharacter* PlayerChar = Cast<ACharacter>(FollowTargetPosition);
    if (!PlayerChar) { return false; }
    FVector PlayerVelocity = PlayerChar->GetVelocity();
    if (PlayerVelocity.Size() < PlayerSpeedThreshold) { return false; }
    
    FVector VectorToCompanion = (GetActorLocation() - PlayerChar->GetActorLocation());
    VectorToCompanion.Normalize();
    FVector PlayerVelDir = PlayerVelocity.GetSafeNormal();

    //kolla infallsvinkel mot spelaren
    float Dot = FVector::DotProduct(PlayerVelDir, VectorToCompanion);
    return (Dot > DotThreshold);
}

void ACompanionCharacter::MoveAwayFromPlayer()
{
    if (!AIController || !FollowTargetPosition) { return; }
    bIsSidestepping = true;
    
    FVector MyLocation = GetActorLocation();
    FVector PlayerLocation = FollowTargetPosition->GetActorLocation();
    FVector AwayDir = (MyLocation - PlayerLocation).GetSafeNormal();

    //testa om den kan gå åtsidan till höger o vänster
    FVector BestSideLoc;
    if (PickBestSideStepLocation(MyLocation, AwayDir, BestSideLoc))
    {
        AIController->MoveToLocation(BestSideLoc, 5.f);
    }
    else
    {
        //fallback till att bara gå bakåt
        BackAwayFromPlayer(MyLocation, AwayDir);
    }
}

bool ACompanionCharacter::PickBestSideStepLocation(
    const FVector& MyLocation, 
    const FVector& AwayDir, 
    FVector& OutBestLoc)
{
    if (AwayDir.IsNearlyZero())
        return false;

    float SideStepAngle = 90.f; // ±90° för att få vänster/höger

    FRotator BaseRot = AwayDir.Rotation();

    //vänster
    FRotator LeftRot = BaseRot + FRotator(0.f, -SideStepAngle, 0.f);
    FVector LeftDir  = LeftRot.Vector();
    FVector LeftLoc  = MyLocation + LeftDir * MoveAwayDistance;

    //höger
    FRotator RightRot = BaseRot + FRotator(0.f, +SideStepAngle, 0.f);
    FVector RightDir  = RightRot.Vector();
    FVector RightLoc  = MyLocation + RightDir * MoveAwayDistance;

    float LeftDist  = GetPathLengthFromTo(MyLocation, LeftLoc);
    float RightDist = GetPathLengthFromTo(MyLocation, RightLoc);

    //om båda -1 => ingen av dem är navigerbar
    if (LeftDist < 0.f && RightDist < 0.f) { return false; }
    
    //bara åt vänster funkar
    if (LeftDist >= 0.f && RightDist < 0.f) { OutBestLoc = LeftLoc; return true; }
    
    //bara höger funkar
    if (LeftDist < 0.f && RightDist >= 0.f) { OutBestLoc = RightLoc; return true; }
    
    //båda funkar => välj kortast eller slumpa om ~ lika
    float Tolerance = 50.f; 
    float Diff = FMath::Abs(LeftDist - RightDist);
        
    if (Diff <= Tolerance)
    {
        bool bChooseLeft = FMath::RandBool();
        OutBestLoc = bChooseLeft ? LeftLoc : RightLoc;
        return true;
    }
    if (LeftDist < RightDist)
    {
        OutBestLoc = LeftLoc;
        return true;
    }
    OutBestLoc = RightLoc;
    return true;
}

float ACompanionCharacter::GetPathLengthFromTo(const FVector& Start, const FVector& End) const
{
    if (!GetWorld()) { return -1.f; }
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys) { return -1.f; }
    UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(GetWorld(), Start, End);
    if (NavPath && NavPath->IsValid()) { return NavPath->GetPathLength(); }
    return -1.f;
}

void ACompanionCharacter::BackAwayFromPlayer(const FVector& MyLocation, const FVector& AwayDir)
{
    if (!AIController) { return; }
    FVector BackLoc = MyLocation + (AwayDir * MoveAwayDistance);

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSys)
    {
        FNavLocation ProjectedLoc;
        if (NavSys->ProjectPointToNavigation(BackLoc, ProjectedLoc, FVector(100.f, 100.f, 100.f)))
        {
            AIController->MoveToLocation(ProjectedLoc.Location, 5.f);
            return;
        }
    }
    //misslyckas, försök ändå
    AIController->MoveToLocation(BackLoc, 5.f);
}
