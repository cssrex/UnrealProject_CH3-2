#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CMPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ACMPlayerPawn;
class ACMPlanePawn;

UCLASS()
class CUSTOMMOVEMENT_API ACMPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void EnterPlane(ACMPlanePawn* Plane);
	void ExitPlane();

public:
	ACMPlayerController();

protected:
	virtual void BeginPlay() override;

public:
	// Player 입력
	UPROPERTY(EditAnywhere, Category = "Inputs")
	TObjectPtr<UInputMappingContext> PlayerInputMappingContext;
	UPROPERTY(EditAnywhere, Category = "Inputs")
	TObjectPtr<UInputAction> PlayerMoveAction;
	UPROPERTY(EditAnywhere, Category = "Inputs")
	TObjectPtr<UInputAction> PlayerLookAction;

	// Plane 입력
	UPROPERTY(EditAnywhere, Category = "Inputs")
	TObjectPtr<UInputMappingContext> PlaneInputMappingContext;
	UPROPERTY(EditAnywhere, Category = "Inputs")
	TObjectPtr<UInputAction> PlaneMoveAction;
	UPROPERTY(EditAnywhere, Category = "Inputs")
	TObjectPtr<UInputAction> PlaneLookAction;

	// 공통 입력
	UPROPERTY(EditAnywhere, Category = "Inputs")
	TObjectPtr<UInputAction> InteractAction;

private:
	void SetPlayerPawnEnabled(bool bEnabled);

	UPROPERTY()
	TObjectPtr<ACMPlayerPawn> PlayerPawn;
	UPROPERTY()
	TObjectPtr<ACMPlanePawn> PlanePawn;
};
