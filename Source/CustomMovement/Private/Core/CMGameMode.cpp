#include "Core/CMGameMode.h"
#include "Player/CMPlayerPawn.h"

ACMGameMode::ACMGameMode()
{
	DefaultPawnClass = ACMPlayerPawn::StaticClass();
}