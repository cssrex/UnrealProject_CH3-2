#include "Player/CMPlayerPawn.h"
#include "Player/CMPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ACMPlayerPawn::ACMPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapusleCollision"));
	CapsuleCollision->SetSimulatePhysics(false);
	SetRootComponent(CapsuleCollision);
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(CapsuleCollision);
	SkeletalMesh->SetSimulatePhysics(false);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(CapsuleCollision);
	SpringArm->TargetArmLength = 300.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	MoveSpeed = 600.0f;
	MouseXSensitive = 4.0f;
	MouseYSensitive = 2.0f;
}

void ACMPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACMPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MoveInput.IsNearlyZero())
	{
		const FVector MoveDirection(MoveInput.X, MoveInput.Y, 0.0f);
		AddActorLocalOffset(MoveDirection * DeltaTime * MoveSpeed, true);
	}
}

void ACMPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ACMPlayerController* PlayerController = Cast<ACMPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ACMPlayerPawn::MoveStart);
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Completed, this, &ACMPlayerPawn::MoveStop);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &ACMPlayerPawn::Look);
			}
		}
	}
}

void ACMPlayerPawn::MoveStart(const FInputActionValue& value)
{
	MoveInput = value.Get<FVector2D>();
}

void ACMPlayerPawn::MoveStop(const FInputActionValue& value)
{
	MoveInput = FVector2D::ZeroVector;
}

void ACMPlayerPawn::Look(const FInputActionValue& value)
{
	const FVector2D LookInput = value.Get<FVector2D>();

	if (!LookInput.IsNearlyZero())
	{
		AddActorLocalRotation(FRotator(0.0f, LookInput.X * MouseXSensitive, 0.0f));
	}

	FRotator SpringArmRotation = SpringArm->GetRelativeRotation();

	SpringArmRotation.Pitch += LookInput.Y * MouseYSensitive;
	SpringArmRotation.Pitch = FMath::Clamp(SpringArmRotation.Pitch, -80.0f, 80.0f);

	SpringArm->SetRelativeRotation(SpringArmRotation);
}

