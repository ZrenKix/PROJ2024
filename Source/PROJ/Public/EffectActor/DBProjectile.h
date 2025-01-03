// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "DBProjectile.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class PROJ_API ADBProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ADBProjectile();
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(BlueprintReadWrite, meta =(ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayImpactSound();
protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	UFUNCTION()
	void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	bool bHit = false;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

};
