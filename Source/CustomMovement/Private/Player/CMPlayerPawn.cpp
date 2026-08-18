#include "Player/CMPlayerPawn.h"
#include "Player/CMPlanePawn.h"
#include "Player/CMPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

FVector ACMPlayerPawn::GetVelocity() const
{
	return CurrentVelocity;
}

bool ACMPlayerPawn::GetShouldMove() const
{
	return CurrentVelocity.Size2D() > 3.0f && !MoveInput.IsNearlyZero();
}

bool ACMPlayerPawn::IsFalling() const
{
	return !bIsGround;
}

ACMPlayerPawn::ACMPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapusleCollision"));
	CapsuleCollision->SetSimulatePhysics(false);
	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CapsuleCollision->SetCollisionObjectType(ECC_Pawn);
	CapsuleCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	CapsuleCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CapsuleCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	SetRootComponent(CapsuleCollision);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetSimulatePhysics(false);
	SphereCollision->SetupAttachment(CapsuleCollision);
	SphereCollision->SetSphereRadius(250.0f);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereCollision->SetGenerateOverlapEvents(true);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(CapsuleCollision);
	SkeletalMesh->SetSimulatePhysics(false);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(CapsuleCollision);
	SpringArm->TargetArmLength = 300.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	MoveSpeed = 600.0f;
	MouseXSensitive = 2.0f;
	MouseYSensitive = 1.0f;
	GravityAcceleration = -980.0f;
	CurrentVelocity = FVector::ZeroVector;
}

void ACMPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ACMPlayerPawn::OnInteractionBeginOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ACMPlayerPawn::OnInteractionEndOverlap);
}

void ACMPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector PreviousLocation = GetActorLocation();

	bIsGround = CheckGround();

	// 앞뒤좌우 움직임 적용
	if (!MoveInput.IsNearlyZero())
	{
		const FVector MoveDirection(MoveInput.X, MoveInput.Y, 0.0f);
		const FVector NormalizedDirection = MoveDirection.GetSafeNormal();
		AddActorLocalOffset(NormalizedDirection * DeltaTime * MoveSpeed, true);
	}

	// 중력 적용
	VerticalVelocity += FVector::UpVector * GravityAcceleration * DeltaTime;

	if (bIsGround && VerticalVelocity.Z < 0.0f)
	{
		VerticalVelocity.Z = 0.0f;
	}

	if (!VerticalVelocity.IsNearlyZero())
	{
		AddActorWorldOffset(VerticalVelocity * DeltaTime, true);
	}

	// 현재 속도 계산
	if (DeltaTime > KINDA_SMALL_NUMBER)
	{
		CurrentVelocity = (GetActorLocation() - PreviousLocation) / DeltaTime;
	}
	else
	{
		CurrentVelocity = FVector::ZeroVector;
	}
}

void ACMPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ACMPlayerController* PlayerController = Cast<ACMPlayerController>(GetController()))
		{
			if (PlayerController->PlayerMoveAction)
			{
				EnhancedInput->BindAction(PlayerController->PlayerMoveAction, ETriggerEvent::Triggered, this, &ACMPlayerPawn::MoveStart);
				EnhancedInput->BindAction(PlayerController->PlayerMoveAction, ETriggerEvent::Completed, this, &ACMPlayerPawn::MoveStop);
			}

			if (PlayerController->PlayerLookAction)
			{
				EnhancedInput->BindAction(PlayerController->PlayerLookAction, ETriggerEvent::Triggered, this, &ACMPlayerPawn::Look);
			}

			if (PlayerController->InteractAction)
			{
				EnhancedInput->BindAction(PlayerController->InteractAction, ETriggerEvent::Started, this, &ACMPlayerPawn::Interact);
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

void ACMPlayerPawn::Interact(const FInputActionValue& value)
{
	if (!NearbyPlane)
	{
		return;
	}

	if (ACMPlayerController* PC = Cast<ACMPlayerController>(GetController()))
	{
		// 비행기 탑승
		PC->EnterPlane(NearbyPlane);
	}
}

void ACMPlayerPawn::OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACMPlanePawn* Plane = Cast<ACMPlanePawn>(OtherActor))
	{
		NearbyPlane = Plane;
	}
}

void ACMPlayerPawn::OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == NearbyPlane)
	{
		NearbyPlane = nullptr;
	}
}

bool ACMPlayerPawn::CheckGround()
{
	if (!GetWorld())
	{
		return false;
	}

	const FVector Start = GetActorLocation();
	const float TraceDistance = CapsuleCollision->GetScaledCapsuleHalfHeight() + 10.0f;
	const FVector End = Start + FVector::DownVector * TraceDistance;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	return GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
}