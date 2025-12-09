// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ShooterCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class USoundCue;
class UNiagaraSystem;
class UAnimMontage;

UCLASS()
class SHOOTEROASIS_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Default Player Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")	
	TObjectPtr<UInputMappingContext> PlayerMappingContext = nullptr;

	// Input Action Move
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction = nullptr;

	// Action to turn - yaw and look up and down together
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAroundAction = nullptr;

	// Action to jump
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction = nullptr;

	// Action when start shooting
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ShootStartAction = nullptr;

	// Action when ending shooting
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ShootEndAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USoundCue> ShootSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> MuzzleFlashNiagara = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HipFireMontage = nullptr;

	// Function to move in all directions
	void Move(const FInputActionValue& Value);

	/* Look up or down base and in yaw by using mouse movement or right gamepad y movement */
	void LookAround(const FInputActionValue& Value);

	// Method call when shooting a weapon
	void ShootButttonPressed();

	// Method call when releasing the shooting button
	void ShootButtonReleased();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/* Camera boom to place camera away from the actor*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraSet, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;

	/* Camera that will be used by the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraSet, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> PlayerCamera = nullptr;

public:
	
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }		// Return CameraBoom
	FORCEINLINE UCameraComponent* GetPlayerCamera() const { return PlayerCamera; }		// Return PlayerCamera


};
