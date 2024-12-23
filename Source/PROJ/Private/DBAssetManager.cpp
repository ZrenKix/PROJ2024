// © 2024 Choklad4Life. All rights reserved.


#include "DBAssetManager.h"

#include "DBGameplayTags.h"

UDBAssetManager* UDBAssetManager::Get()
{
	check(GEngine);

	UDBAssetManager* AssetManager = Cast<UDBAssetManager>(GEngine->AssetManager);

	return AssetManager;
}

void UDBAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	

	FDBGameplayTags::InitializeNativeGameplayTags();
}
