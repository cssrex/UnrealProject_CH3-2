#include "Player/CMPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Player/CMPlayerPawn.h"
#include "Player/CMPlanePawn.h"

void ACMPlayerController::EnterPlane(ACMPlanePawn* Plane)
{
	if (!Plane)
	{
		return;
	}

	PlayerPawn = Cast<ACMPlayerPawn>(GetPawn());

	if (!PlayerPawn)
	{
		return;
	}

	PlanePawn = Plane;

	PlayerPawn->SetActorHiddenInGame(true);
	PlayerPawn->SetActorEnableCollision(false);

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (PlaneInputMappingContext)
			{
				Subsystem->RemoveMappingContext(PlayerInputMappingContext);
				Subsystem->AddMappingContext(PlaneInputMappingContext, 0);
			}
		}
	}

	Possess(PlanePawn);
}

void ACMPlayerController::ExitPlane()
{
	if (!PlayerPawn || !PlanePawn)
	{
		return;

	}

	const FVector ExitLocation = PlanePawn->GetActorLocation() + PlanePawn->GetActorUpVector() * 50.0f + PlanePawn->GetActorRightVector() * 200.0f;

	PlayerPawn->SetActorLocation(ExitLocation);
	PlayerPawn->SetActorHiddenInGame(false);
	PlayerPawn->SetActorEnableCollision(true);

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (PlayerInputMappingContext)
			{
				Subsystem->RemoveMappingContext(PlaneInputMappingContext);
				Subsystem->AddMappingContext(PlayerInputMappingContext, 0);
			}
		}
	}

	Possess(PlayerPawn);

	PlanePawn = nullptr;
}

ACMPlayerController::ACMPlayerController() :
	PlayerInputMappingContext(nullptr),
	PlayerMoveAction(nullptr),
	PlayerLookAction(nullptr)
{
}

void ACMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (PlayerInputMappingContext)
			{
				Subsystem->AddMappingContext(PlayerInputMappingContext, 0);
			}
		}
	}
}
