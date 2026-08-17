#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CMPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class CUSTOMMOVEMENT_API ACMPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACMPlayerController();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = "Inputs")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	TObjectPtr<UInputAction> LookAction;

};
