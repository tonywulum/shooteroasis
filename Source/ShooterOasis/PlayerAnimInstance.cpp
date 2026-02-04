// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (APawn* PlayerPawn = TryGetPawnOwner())
	{
		ShooterCharacter = Cast<AShooterCharacter>(PlayerPawn);
		MoveComp = ShooterCharacter ? ShooterCharacter->GetCharacterMovement() : nullptr;
	}
}

void UPlayerAnimInstance::UpdateAnimProperties(float DeltaTime)
{
	APawn* OwnerNow = TryGetPawnOwner();
	if (OwnerNow != ShooterCharacter || !IsValid(MoveComp))
	{
		ShooterCharacter = Cast<AShooterCharacter>(OwnerNow);
		MoveComp = ShooterCharacter ? ShooterCharacter->GetCharacterMovement() : nullptr;
	}

	if (!ShooterCharacter || !MoveComp) return;

	const FVector Vel = ShooterCharacter->GetVelocity();
	Speed = Vel.Size2D();	// I don't need the component on Z to get the speed

	// Is in the air?
	bIsInAir = MoveComp->IsFalling();

	// Is the player actively providing movement input?
	bIsAccelerating = MoveComp->GetCurrentAcceleration().SizeSquared() > KINDA_SMALL_NUMBER;

	// Is the character moving (has a speed greater than 0)
	bIsMoving = Vel.SizeSquared2D() > KINDA_SMALL_NUMBER;

	const FVector Vel2D = FVector(Vel.X, Vel.Y, 0.0f);
	const bool bHasMoveDir = Vel2D.SizeSquared() > 25.f; // To avoid problems when the speed is very low

	FRotator AimRot = ShooterCharacter->GetControlRotation();
	AimRot.Pitch = 0.0f;
	AimRot.Roll = 0.0f;

	if (bHasMoveDir)
	{
		FRotator MoveRot = Vel2D.Rotation();
		MoveRot.Pitch = 0.0f;
		MoveRot.Roll = 0.0f;

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MoveRot, AimRot).Yaw;

		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, 
			//	FString::Printf(TEXT("Move Rotation: %f"), MoveRot.Yaw));
			//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, 
			//	FString::Printf(TEXT("Aim Rotation: %f"), AimRot.Yaw));

			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, 
				FString::Printf(TEXT("MovementOffsetYaw: %f"), MovementOffsetYaw));
		}
	}

}
