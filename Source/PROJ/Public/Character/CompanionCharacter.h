#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/CompanionAIController.h"
#include "CompanionCharacter.generated.h"


UENUM(BlueprintType)
enum class ECompanionType : uint8
{
    Vincent,
    Teddy
};

UCLASS()
class PROJ_API ACompanionCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ACompanionCharacter();
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion")
    float MovementSpeed = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion")
    float followDistanceThreshold = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion")
    float OffsetDistance = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion")
    ECompanionType CompanionType = ECompanionType::Vincent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion")
    bool bCanFollow = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion")
    bool bIsComforted = false;
    
    FTimerHandle DistanceCheckTimerHandle;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion")
    float DistanceCheckInterval = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion")
    float MaxAllowedDistance = 2000.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion|PushAway")
    float MinComfortDistance = 150.f; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion|PushAway")
    float MoveAwayDistance = 100.f; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion|PushAway")
    float DotThreshold = 0.95f;      

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion|PushAway")
    float PlayerSpeedThreshold = 10.f;  
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion")
    float PushDuration = 0.5f;

    bool bIsBeingPushed = false;
    FVector PushStartLocation;
    FVector PushTargetLocation;
    float PushElapsedTime = 0.f;

    void UpdateVincentFearLogic();
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void PossessedBy(AController* NewController) override;

    UFUNCTION(BlueprintCallable)
    void FollowTargetActor(AActor* Actor);
    UFUNCTION(BlueprintCallable)
    void SetOffsetDistance(float Distance);
    UFUNCTION(BlueprintCallable)
    void SetFollowTarget(AActor* Target);
    bool IsTargetStopped(AActor* Target, float SpeedThreshold = 5.f) const;

    UFUNCTION(BlueprintCallable)
    void MoveToSideOfTarget(AActor* Target);
    void TeleportIfTooFar();
    void CheckDistancePeriodically();
    float GetClosestEnemyDistance();
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Companion")
    ACompanionAIController* AIController = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Companion")
    AActor* FollowTargetPosition = nullptr;

    UPROPERTY(BlueprintReadWrite, Category="Companion")
    bool bInArena = false;

protected:
    UPROPERTY()
    ACharacter* CachedPlayerRef = nullptr;
    
    float LastAvoidTime = 0.f;
    UPROPERTY(EditAnywhere, Category="Companion|PushAway")
    float AvoidCooldown = 1.0f;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Companion")
    bool bIsSidestepping = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Companion")
    float PlayerStillTime = 0.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Companion")
    float RequiredStillTime = 1.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Companion")
    bool bHasMovedToSideDuringStill = false;
    
    bool IsPlayerMovingTowardMe() const;
    void MoveAwayFromPlayer();
    bool PickBestSideStepLocation(const FVector& MyLocation, const FVector& AwayDir, FVector& OutBestLoc);
    float GetPathLengthFromTo(const FVector& Start, const FVector& End) const;
    void BackAwayFromPlayer(const FVector& MyLocation, const FVector& AwayDir);

};
