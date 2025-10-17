// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}
