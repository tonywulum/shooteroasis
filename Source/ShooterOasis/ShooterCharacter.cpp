// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/SkeletalMeshSocket.h"

#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"


// Sets default values
AShooterCharacter::AShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	PlayerCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.2f;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}

}

void AShooterCharacter::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		// Get a 2D vector (x = forward/backward, y = right/left)
		FVector2D MovementVector = Value.Get<FVector2D>();

		// Get Forward and Right Direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Adding Movement Forward or Right
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AShooterCharacter::LookAround(const FInputActionValue& Value)
{
	const FVector2D LookAroundVector = Value.Get<FVector2D>();
	const float TurnScaleFactor = 1.0f;
	AddControllerYawInput(LookAroundVector.X * TurnScaleFactor);

	const float LookUpScaleFactor = 1.0f;
	AddControllerPitchInput(LookAroundVector.Y * LookUpScaleFactor);
}

void AShooterCharacter::ShootButttonPressed()
{
	PlayShootSfx();

	FTransform SocketTransform;
	FVector MuzzleStart;
	if (!TryGetMuzzleTransform(SocketTransform, MuzzleStart)) return;
	
	SpawnMuzzleFlash(MuzzleStart, SocketTransform);

	FVector AimPoint;
	if (!TryGetCrosshairAimPoint(AimPoint)) return;

	FHitResult MuzzleHit;
	const FVector FinalImpactPoint = ResolveMuzzleToAim(MuzzleStart, AimPoint, MuzzleHit);
	
	SpawnBeamVfx(MuzzleStart, FinalImpactPoint, SocketTransform);
	if (MuzzleHit.bBlockingHit)
	{
		SpawnImpactVfxAndDecal(MuzzleHit);
	}

	PlayFireMontage();
}

void AShooterCharacter::ShootButtonReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("Shoot button released"));
}

void AShooterCharacter::OnAimStarted()
{
	bIsAiming = true;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Aiming started"));
	}
}

void AShooterCharacter::OnAimReleased()
{
	bIsAiming = false;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Aiming ended"));
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Move);
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &AShooterCharacter::LookAround);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AShooterCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AShooterCharacter::StopJumping);

		EnhancedInputComponent->BindAction(ShootStartAction, ETriggerEvent::Triggered, this, &AShooterCharacter::ShootButttonPressed);
		EnhancedInputComponent->BindAction(ShootEndAction, ETriggerEvent::Triggered, this, &AShooterCharacter::ShootButtonReleased);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AShooterCharacter::OnAimStarted);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AShooterCharacter::OnAimReleased);

	}

}

void AShooterCharacter::PlayShootSfx() const
{
	if (ShootSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShootSound, GetActorLocation());
	}
}

bool AShooterCharacter::TryGetMuzzleTransform(FTransform& OutSocketTransform, FVector& OutMuzzleStart) const
{
	const USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return false;

	const USkeletalMeshSocket* BarrelSocket = MeshComp->GetSocketByName(TEXT("BarrelSocket"));
	if (!BarrelSocket) return false;

	OutSocketTransform = BarrelSocket->GetSocketTransform(MeshComp);
	OutMuzzleStart = OutSocketTransform.GetLocation();

	return true;
}

void AShooterCharacter::SpawnMuzzleFlash(const FVector& MuzzleStart, const FTransform& SocketTransform) const
{
	if (MuzzleFlashNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashNiagara, MuzzleStart, SocketTransform.GetRotation().Rotator());
	}
}

bool AShooterCharacter::TryGetCrosshairAimPoint(FVector& OutAimPoint) const
{
	// Get Current Size of the Viewport
	if (!GEngine && GEngine->GameViewport) return false;

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	// Get Screen Location of Crosshair
	const FVector2D CrosshairLocation = FVector2D(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	// Get Player Controller
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return false;

	// De-project the Screen Position of the Crosshair to a World Direction
	FVector CameraWorldPos;
	FVector CameraWorldDir;
	if (!UGameplayStatics::DeprojectScreenToWorld(PC, CrosshairLocation, CameraWorldPos, CameraWorldDir)) return false;

	// Setup Trace Parameters
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ShootTrace), true);
	Params.AddIgnoredActor(this);

	// Trace from camera along crosshair world direction
	const float TraceRange = 50'000.f;
	const FVector AimTraceStart = CameraWorldPos;
	const FVector AimTraceEnd = AimTraceStart + (CameraWorldDir * TraceRange);

	FHitResult AimHit;
	const bool bAimHit = GetWorld()->LineTraceSingleByChannel(AimHit, AimTraceStart, AimTraceEnd, ECC_Visibility, Params);

	OutAimPoint = bAimHit ? AimHit.ImpactPoint : AimTraceEnd;

	return true;
}

FVector AShooterCharacter::ResolveMuzzleToAim(const FVector& MuzzleStart, const FVector& AimPoint, FHitResult& OutHit) const
{
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ShootTrace), true);
	Params.AddIgnoredActor(this);

	const float MuzzleTraceRadius = 3.0f;

	const bool bMuzzleHit = GetWorld()->SweepSingleByChannel(OutHit, MuzzleStart, AimPoint, FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeSphere(MuzzleTraceRadius), Params);

	const bool bMuzzleBlockingHit = bMuzzleHit && OutHit.bBlockingHit;
	return bMuzzleBlockingHit ? OutHit.ImpactPoint : AimPoint;
}

void AShooterCharacter::SpawnBeamVfx(const FVector& Start, const FVector& Target, const FTransform& SocketTransform) const
{
	// Beam VFX: muzzle to impact point
	if (!BulletBeamNiagara) return;

	UNiagaraComponent* BeamComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletBeamNiagara,
		Start, SocketTransform.Rotator());

	if (BeamComp)
	{
		BeamComp->SetVectorParameter(FName("Start"), Start);
		BeamComp->SetVectorParameter(FName("Target"), Target);
	}
}

void AShooterCharacter::SpawnImpactVfxAndDecal(const FHitResult& Hit) const
{
	// Impact VFX + decal only if we hit something
	if (ImpactNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactNiagara, Hit.ImpactPoint,
			Hit.ImpactNormal.Rotation());
	}
	if (ImpactDecalMat)
	{
		const FVector N = Hit.ImpactNormal.GetSafeNormal();
		UGameplayStatics::SpawnDecalAtLocation(GetWorld(), ImpactDecalMat, ImpactDecalSize,
			Hit.ImpactPoint + N, (-N).Rotation(), 20.f);
	}
}

void AShooterCharacter::PlayFireMontage() const
{
	static const FName BeginFireSectionName = FName("BeginFire");
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (HipFireMontage)
		{
			if (!AnimInstance->Montage_IsPlaying(HipFireMontage))
			{
				AnimInstance->Montage_Play(HipFireMontage);
			}
			AnimInstance->Montage_JumpToSection(BeginFireSectionName, HipFireMontage);
		}
	}
}

