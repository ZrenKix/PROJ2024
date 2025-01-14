// UnitPaperBase.h

#pragma once

#include "CoreMinimal.h"
#include "Arena/Unit/UnitBase.h"
#include "PaperZDAnimationComponent.h"
#include "PaperZDAnimInstance.h"
#include "PaperFlipbookComponent.h"
#include "Interaction/TargetInterface.h"
#include "Sequencer/IPaperZDSequencerSource.h"
#include "UnitPaperBase.generated.h"

UCLASS()
class PROJ_API AUnitPaperBase : public AUnitBase, public IPaperZDSequencerSource, public ITargetInterface
{
	GENERATED_BODY()

private:
	/** Animation component that manages the AnimBP. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PaperZD", meta = (AllowPrivateAccess = "true"))
	UPaperZDAnimationComponent* AnimationComponent;

	/** The AnimInstance that drives this character. */
	UPROPERTY(Transient, VisibleInstanceOnly, Category = "PaperZD", BlueprintGetter = "GetAnimInstance")
	UPaperZDAnimInstance* AnimInstance;

	/** The main flipbook component associated with this character. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	UPaperFlipbookComponent* Sprite;

public:
	AUnitPaperBase();

	// Overrides
	virtual void PostInitializeComponents() override;

	// IPaperZDSequencerSource Interface
	virtual TSubclassOf<UPaperZDAnimInstance> GetSequencerAnimInstanceClass() const override;
	virtual UPaperZDAnimInstance* GetSequencerAnimInstance() override;

	/** Getter for the AnimInstance */
	UFUNCTION(BlueprintPure, Category = "PaperZD")
	UPaperZDAnimInstance* GetAnimInstance() const;

	/** Getter for the AnimationComponent */
	FORCEINLINE UPaperZDAnimationComponent* GetAnimationComponent() const { return AnimationComponent; }

	/** Getter for the Sprite component */
	FORCEINLINE UPaperFlipbookComponent* GetSprite() const { return Sprite; }

	virtual void OnTargeted_Implementation() override;
	virtual void OnTargetedEnd_Implementation() override;

	UFUNCTION(BlueprintCallable)
	void OnTargetSet() const;

	UFUNCTION(BlueprintCallable)
	void ClearTarget() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> TargetDecal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> OnTargetSetDecal;
};
