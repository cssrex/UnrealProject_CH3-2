#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CMPlanePawn.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;

struct FInputActionValue;

UCLASS()
class CUSTOMMOVEMENT_API ACMPlanePawn : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float UpDownAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float MaxUpDownSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float MouseXSensitive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float MouseYSensitive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float MouseZSensitive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float GravityAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float AirControlRatio;

public:
	ACMPlanePawn();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UFUNCTION()
	void MoveStart(const FInputActionValue& value);
	UFUNCTION()
	void MoveStop(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void Interact(const FInputActionValue& value);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Item|Components")
	TObjectPtr<UBoxComponent> BoxCollision;

	UPROPERTY(VisibleAnywhere, Category = "Item|Components")
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(VisibleAnywhere, Category = "Item|Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Item|Components")
	TObjectPtr<UCameraComponent> Camera;

private:
	bool CheckGround();

	FVector MoveInput;
	FVector VerticalVelocity;
	bool bIsGround;
};
