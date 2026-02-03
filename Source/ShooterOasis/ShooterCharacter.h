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

	// Action to aim
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AimAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USoundCue> ShootSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> MuzzleFlashNiagara = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HipFireMontage = nullptr;

	/* Bullet impact Niagara System on Surface*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> ImpactNiagara = nullptr;

	/* Add Decal Over Impacted Surface */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> ImpactDecalMat = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, meta = (AllowPrivateAccess = "true"))
	FVector ImpactDecalSize = FVector(8.f, 8.f, 8.f);	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, meta = (AllowPrivateAccess = "true"))
	float ImpactDecalLifeSpan = 5.f;
	
	/* Bullet Beam Niagara System */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> BulletBeamNiagara = nullptr;

	// Is Aiming
	UPROPERTY(BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming = false;

	// Function to move in all directions
	void Move(const FInputActionValue& Value);

	/* Look up or down base and in yaw by using mouse movement or right gamepad y movement */
	void LookAround(const FInputActionValue& Value);

	// Method call when shooting a weapon
	void ShootButttonPressed();

	// Method call when releasing the shooting button
	void ShootButtonReleased();

	// Method call when aiming started
	void OnAimStarted();

	// Method call when aiming ended
	void OnAimReleased();

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

	// Play shooting sound effect
	void PlayShootSfx() const;

	// Try to get the muzzle transform and muzzle start location
	bool TryGetMuzzleTransform(FTransform& OutSocketTransform, FVector& OutMuzzleStart) const;
	// Spawn Muzzle Flash at the given location and rotation
	void SpawnMuzzleFlash(const FVector& MuzzleStart, const FTransform& SocketTransform) const;

	// Try to get the crosshair aim point in the world
	bool TryGetCrosshairAimPoint(FVector& OutAimPoint) const;
	// Resolve the muzzle to aim point, return the hit result
	FVector ResolveMuzzleToAim(const FVector& MuzzleStart, const FVector& AimPoint, FHitResult& OutHit) const;

	// Spawn beam VFX from start to target 
	void SpawnBeamVfx(const FVector& Start, const FVector& Target, const FTransform& SocketTransform) const;
	// Spawn impact effects and decal at the hit location
	void SpawnImpactVfxAndDecal(const FHitResult& Hit) const;

	// Play fire montage
	void PlayFireMontage() const;

public:
	
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }		// Return CameraBoom
	FORCEINLINE UCameraComponent* GetPlayerCamera() const { return PlayerCamera; }		// Return PlayerCamera


};
