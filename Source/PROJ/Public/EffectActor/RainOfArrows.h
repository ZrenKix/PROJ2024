// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RainOfArrows.generated.h"

UCLASS()
class PROJ_API ARainOfArrows : public AActor
{
	GENERATED_BODY()
	
public:	
	ARainOfArrows();
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> ArrowCircleDecal;
protected:
	virtual void BeginPlay() override;


};
