// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

class AShooterCharacter;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class SHOOTEROASIS_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable)
	void UpdateAnimProperties(float DeltaTime);

private:

	// Character using the animation
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AShooterCharacter> ShooterCharacter = nullptr;

	// Movement Component of Character using the animation
	UPROPERTY(Transient)
	TObjectPtr<UCharacterMovementComponent> MoveComp = nullptr;

	// Says if the character is in the air because is jumping or falling
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir = false;

	// It's indicanting that the character is changing the speed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating = false;

	// It's indicating that the character is moving (speed different to zero)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess = "true"))
	bool bIsMoving = false;

	// Movement speed of the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess = "true"))
	float Speed = 0.0f;

};
