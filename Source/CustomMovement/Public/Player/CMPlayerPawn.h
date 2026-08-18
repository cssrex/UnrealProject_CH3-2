#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CMPlayerPawn.generated.h"

class UCapsuleComponent;
class USphereComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class ACMPlanePawn;

struct FInputActionValue;

UCLASS()
class CUSTOMMOVEMENT_API ACMPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// 애니메이션 전용 함수
	virtual FVector GetVelocity() const override;
	UFUNCTION(BlueprintCallable)
	bool GetShouldMove() const;
	UFUNCTION(BlueprintCallable)
	bool IsFalling() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Value")
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Value")
	float GravityAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Input")
	float MouseXSensitive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Input")
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
	UFUNCTION()
	void Interact(const FInputActionValue& value);

protected:
	UFUNCTION()
	void OnInteractionBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnInteractionEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Item|Components")
	TObjectPtr<UCapsuleComponent> CapsuleCollision;

	UPROPERTY(VisibleAnywhere, Category = "Item|Components")
	TObjectPtr<USphereComponent> SphereCollision;

	UPROPERTY(VisibleAnywhere, Category = "Item|Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(VisibleAnywhere, Category = "Item|Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Item|Components")
	TObjectPtr<UCameraComponent> Camera;

private:
	bool CheckGround();

	FVector2D MoveInput;
	FVector VerticalVelocity;
	FVector CurrentVelocity;
	bool bIsGround;

	UPROPERTY()
	TObjectPtr<ACMPlanePawn> NearbyPlane;
};
