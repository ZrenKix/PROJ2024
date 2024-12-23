// © 2024 Choklad4Life. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "DBAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UDBAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UDBAssetManager* Get();

protected:
	virtual void StartInitialLoading() override;
	
};
