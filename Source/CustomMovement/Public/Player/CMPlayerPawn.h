#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CMPlayerPawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;

struct FInputActionValue;

UCLASS()
class CUSTOMMOVEMENT_API ACMPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float MouseXSensitive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float MouseYSensitive;

public:
	ACMPlayerPawn();

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

protected:
	UPROPERTY(VisibleAnywhere, Category = "Item|Components")
	TObjectPtr<UCapsuleComponent> CapsuleCollision;

	UPROPERTY(VisibleAnywhere, Category = "Item|Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(VisibleAnywhere, Category = "Item|Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Item|Components")
	TObjectPtr<UCameraComponent> Camera;

private:
	FVector2D MoveInput;

};
