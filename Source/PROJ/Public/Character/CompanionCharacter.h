// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CompanionAIController.h"
#include "GameFramework/Character.h"
#include "Interaction/InteractableInterface.h"
#include "CompanionCharacter.generated.h"

UENUM(BlueprintType)
enum class ECompanionType : uint8
{
	Vincent UMETA(DisplayName="Vincent"),
	Teddy   UMETA(DisplayName="Teddy")
};

UCLASS()
class PROJ_API ACompanionCharacter : public ACharacter, public IInteractableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACompanionCharacter();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion")
	ECompanionType CompanionType;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Yeet();
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetFollowTarget(AActor* target);

	UFUNCTION(BlueprintCallable)
	void SetOffsetDistance(float Distance);

	UFUNCTION()
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable)
	float GetClosestEnemyDistance();

	UFUNCTION(BlueprintCallable)
	void FollowTargetActor(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void UpdatePush(float DeltaTime);
	
	UPROPERTY()
	ACompanionAIController* AIController;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	float followDistanceThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	float OffsetDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	float RotationSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	float MovementSpeed = 300.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Companion|Push")
	FVector PushStartLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Companion|Push")
	FVector PushTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Companion|Push")
	float PushElapsedTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion|Push")
	float PushDuration = 0.7;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Companion|Push")
	bool bIsBeingPushed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	bool bCanFollow = false;
	UPROPERTY(BlueprintReadWrite, Category="Companion|State")
	bool bIsComforted = false;
	
private:
	UPROPERTY()
	AActor* FollowTargetPosition;

	UPROPERTY()
	FTimerHandle AvoidanceCooldownTimer;

};
