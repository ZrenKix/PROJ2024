#include "DreamboundGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Controllers/ArenaPlayerController.h"
#include "Player/Controllers/WorldPlayerController.h"

ADreamboundGameMode::ADreamboundGameMode()
{
    PlayerControllerClass = AWorldPlayerController::StaticClass();
}

void ADreamboundGameMode::SwitchToArenaController()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        PlayerController->UnPossess();
        
        PlayerControllerClass = AArenaPlayerController::StaticClass();
    }
}

void ADreamboundGameMode::SwitchToWorldController()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        PlayerController->UnPossess();
        
        PlayerControllerClass = AWorldPlayerController::StaticClass();
    }
}
