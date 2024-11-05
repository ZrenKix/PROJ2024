// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjCharacterBase.generated.h"

UCLASS()
class PROJ_API AProjCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AProjCharacterBase();

protected:
	virtual void BeginPlay() override;

};
