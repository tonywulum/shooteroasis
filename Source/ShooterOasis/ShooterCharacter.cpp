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

	if (ShootSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShootSound, GetActorLocation());
	}

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName(TEXT("BarrelSocket"));
	if (!BarrelSocket) return;

	const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
	const FVector TraceStart = SocketTransform.GetLocation();
	const FVector TraceEnd = TraceStart + (SocketTransform.GetRotation().GetForwardVector() * 10000.f);

	if (MuzzleFlashNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashNiagara, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());
	}

	// Trace (ignore self)
	FHitResult HitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ShootTrace), true);
	Params.AddIgnoredActor(this);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);

	const FVector End = bHit ? HitResult.Location : TraceEnd;

	// Spawn beam (instant tracer)
	if (BulletBeamNiagara)
	{
		UNiagaraComponent* BeamComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletBeamNiagara, TraceStart, SocketTransform.Rotator());
		
		if (BeamComp)
		{
			BeamComp->SetVectorParameter(FName("Start"), TraceStart);
			BeamComp->SetVectorParameter(FName("Target"), End);
		}
	}

	// Impact VFX + decal only if we hit something
	if (bHit)
	{
		if (ImpactNiagara)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactNiagara, HitResult.Location, HitResult.ImpactNormal.Rotation());
		}
		if (ImpactDecalMat)
		{
			const FVector N = HitResult.ImpactNormal.GetSafeNormal();

			UGameplayStatics::SpawnDecalAtLocation(GetWorld(), ImpactDecalMat, ImpactDecalSize, HitResult.ImpactPoint + N, (-N).Rotation(), 20.f);
		}
	}

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

