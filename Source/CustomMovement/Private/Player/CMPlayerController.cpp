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

	// 플레이어 Pawn 숨기기
	SetPlayerPawnEnabled(false);

	// 비행기 입력으로 전환
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
	
	// 비행기 Pawn에 빙의
	Possess(PlanePawn);
}

void ACMPlayerController::ExitPlane()
{
	if (!PlayerPawn || !PlanePawn)
	{
		return;

	}

	const FVector ExitLocation = PlanePawn->GetActorLocation() + PlanePawn->GetActorUpVector() * 50.0f + PlanePawn->GetActorRightVector() * 200.0f;

	// 플레이어 Pawn 위치를 비행기 옆으로 이동
	PlayerPawn->SetActorLocation(ExitLocation);

	// 플레이어 Pawn 나타내기
	SetPlayerPawnEnabled(true);

	// 플레이어 입력으로 전환
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

	// 플레이어 Pawn에 빙의
	Possess(PlayerPawn);

	PlanePawn = nullptr;
}

ACMPlayerController::ACMPlayerController() :
	PlayerInputMappingContext(nullptr),
	PlayerMoveAction(nullptr),
	PlayerLookAction(nullptr),
	PlaneInputMappingContext(nullptr),
	PlaneMoveAction(nullptr),
	PlaneLookAction(nullptr)
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

void ACMPlayerController::SetPlayerPawnEnabled(bool bEnabled)
{
	if (!PlayerPawn)
	{
		return;
	}

	PlayerPawn->SetActorHiddenInGame(!bEnabled);
	PlayerPawn->SetActorEnableCollision(bEnabled);
}
