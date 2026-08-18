#include "Player/CMPlanePawn.h"
#include "Player/CMPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ACMPlanePawn::ACMPlanePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetSimulatePhysics(false);
	BoxCollision->SetBoxExtent(FVector(150.0f, 100.0f, 50.0f));
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision->SetCollisionObjectType(ECC_Pawn);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn,ECR_Block);
	BoxCollision->SetGenerateOverlapEvents(true);
	SetRootComponent(BoxCollision);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(BoxCollision);
	StaticMesh->SetSimulatePhysics(false);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(BoxCollision);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	MoveInput = FVector::ZeroVector;
	VerticalVelocity = FVector::ZeroVector;
	bIsGround = false;

	MoveSpeed = 1500.0f;
	UpDownAcceleration = 1500.0f;

	MouseXSensitive = 2.0f;
	MouseYSensitive = 1.0f;
	MouseZSensitive = 10.0f;

	GravityAcceleration = -980.0f;
	AirControlRatio = 0.4f;
}

void ACMPlanePawn::BeginPlay()
{
	Super::BeginPlay();
}

void ACMPlanePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bIsGround = CheckGround();

	// 앞뒤좌우 움직임 적용
	const FVector MoveDirection(MoveInput.X, MoveInput.Y, 0.0f);

	if (!MoveDirection.IsNearlyZero())
	{
		const FVector NormalizedDirection = MoveDirection.GetSafeNormal();
		const float CurrentMoveSpeed = bIsGround ? MoveSpeed : MoveSpeed * AirControlRatio;
		AddActorLocalOffset(NormalizedDirection * DeltaTime * CurrentMoveSpeed, true);
	}

	// 중력 + 상하 움직임 적용
	if (!FMath::IsNearlyZero(MoveInput.Z))
	{
		VerticalVelocity += FVector::UpVector * MoveInput.Z * UpDownAcceleration * DeltaTime;
	}

	VerticalVelocity += FVector::UpVector * GravityAcceleration * DeltaTime;
	
	if (bIsGround && VerticalVelocity.Z < 0.0f)
	{
		VerticalVelocity.Z = 0.0f;
	}

	if (!VerticalVelocity.IsNearlyZero())
	{
		AddActorWorldOffset(VerticalVelocity * DeltaTime, true);
	}
}

void ACMPlanePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ACMPlayerController* PlayerController = Cast<ACMPlayerController>(GetController()))
		{
			if (PlayerController->PlaneMoveAction)
			{
				EnhancedInput->BindAction(PlayerController->PlaneMoveAction, ETriggerEvent::Triggered, this, &ACMPlanePawn::MoveStart);
				EnhancedInput->BindAction(PlayerController->PlaneMoveAction, ETriggerEvent::Completed, this, &ACMPlanePawn::MoveStop);
			}

			if (PlayerController->PlaneLookAction)
			{
				EnhancedInput->BindAction(PlayerController->PlaneLookAction, ETriggerEvent::Triggered, this, &ACMPlanePawn::Look);
			}

			if (PlayerController->InteractAction)
			{
				EnhancedInput->BindAction(PlayerController->InteractAction, ETriggerEvent::Started, this, &ACMPlanePawn::Interact);
			}
		}
	}
}

void ACMPlanePawn::MoveStart(const FInputActionValue& value)
{
	MoveInput = value.Get<FVector>();
}

void ACMPlanePawn::MoveStop(const FInputActionValue& value)
{
	MoveInput = FVector::ZeroVector;
}

void ACMPlanePawn::Look(const FInputActionValue& value)
{
	const FVector LookInput = value.Get<FVector>();

	// 마우스 좌우(Yaw) : Actor 전체 회전
	if (!FMath::IsNearlyZero(LookInput.X))
	{
		AddActorLocalRotation(FRotator(0.0f, LookInput.X * MouseXSensitive, 0.0f));
	}

	// 마우스 상하(Pitch) : SpringArm 회전
	if (!FMath::IsNearlyZero(LookInput.Y))
	{
		FRotator SpringArmRotation = SpringArm->GetRelativeRotation();

		SpringArmRotation.Pitch += LookInput.Y * MouseYSensitive;
		SpringArmRotation.Pitch = FMath::Clamp(SpringArmRotation.Pitch, -80.0f, 80.0f);

		SpringArm->SetRelativeRotation(SpringArmRotation);
	}

	// 마우스 휠(Roll) : StaticMesh 회전
	if (!FMath::IsNearlyZero(LookInput.Z))
	{
		StaticMesh->AddLocalRotation(FRotator(0.0f, 0.0f, LookInput.Z * MouseZSensitive));
	}
}

void ACMPlanePawn::Interact(const FInputActionValue& value)
{
	if (ACMPlayerController* PC = Cast<ACMPlayerController>(GetController()))
	{
		// 비행기 내리기
		PC->ExitPlane();
	}
}

bool ACMPlanePawn::CheckGround()
{
	if (!GetWorld())
	{
		return false;
	}

	const FVector Start = GetActorLocation();
	const float TraceDistance = BoxCollision->GetScaledBoxExtent().Z + 10.0f;
	const FVector End = Start + FVector::DownVector * TraceDistance;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	return GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
}
