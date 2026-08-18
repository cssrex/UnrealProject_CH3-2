#include "Core/CMGameMode.h"
#include "Player/CMPlayerPawn.h"
#include "Player/CMPlayerController.h"

ACMGameMode::ACMGameMode()
{
	DefaultPawnClass = ACMPlayerPawn::StaticClass();
	PlayerControllerClass = ACMPlayerController::StaticClass();
}