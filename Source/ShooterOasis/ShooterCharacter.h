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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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

	// Yaw and Pitch sensitivity when NOT aiming, aka hip firing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSet", meta = (AllowPrivateAccess = "true"))
	float HipYawSensitivity = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSet", meta = (AllowPrivateAccess = "true"))
	float HipPitchSensitivity = 1.f;

	// Yaw and Pitch sensitivity when aiming, usually smaller than hip firing sensitivity for better control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSet", meta = (AllowPrivateAccess = "true"))
	float AimYawSensitivity = 0.35f;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSet", meta = (AllowPrivateAccess = "true"))
	float AimPitchSensitivity = 0.35f;

	// Method call when shooting a weapon
	void ShootButttonPressed();

	// Method call when releasing the shooting button
	void ShootButtonReleased();

	// Method call when aiming started
	void OnAimStarted();

	// Method call when aiming ended
	void OnAimReleased();

	// Default Field Of View when not aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSet, meta = (AllowPrivate = "true"))
	float CameraDefaultFOV = 90.f;

	// Default Field Of View when aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSet, meta = (AllowPrivate = "true"))
	float AimFOV = 60.f;

	// Interp speed for FOV change
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSet, meta = (AllowPrivate = "true"))
	float FOVInterpSpeed = 20.f;

public:	

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

	

	// Current FOV, used for smooth transition between default FOV and aim FOV
	float CurrentFOV;

	// Update the camera's FOV smoothly when aiming or not aiming
	void UpdateCameraFOV(float DeltaTime);
	
	// Update all crosshairs spread values every frame
	void UpdateCrosshairSpread(float DeltaTime);

	// Update spread caused by movement speed
	void UpdateMovementSpread();

	// Update spread caused by being in the air
	void UpdateInAirSpread(float DeltaTime);

	// Recover temporary shooting spread over time
	void RecoverShootingSpread(float DeltaTime);

	// Combine all spread contributions into the final current spread
	void CalculateCurrentCrosshairSpread();

	// Add spread when the player fires
	void AddShootingSpread();

	// Return horizontal speed normalized from 0 to 1;
	float GetNormalizedMovementSpeed() const;

	// Base spread always present, even while standing still
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float BaseCrosshairSpread = 8.0f;

	// Final spread used by the HUD
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CurrentCrosshairSpread = 0.0f;

	// Current spread contribution from movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float MovementSpread = 0.0f;

	// Current spread contribution from being in the air
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float InAirSpread = 0.0f;

	// Current temporary spread contribution from firing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float ShootingSpread = 0.0f;

	// Maximum movement spread
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float MaxMovementSpread = 12.0f;

	// Max Spread penalty while in the air
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float MaxInAirSpread = 18.0f;

	// Spread added every time the player fires
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float ShootingSpreadIncrement = 4.0f;

	// Maximum shooting spread that can accumulate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float MaxShootingSpread = 16.0f;

	// Recovery speed for shooting spread
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float SpreadRecoverySpeed = 20.0f;

	// Multiplier applied when aiming
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float AimSpreadMultiplier = 0.6f;

	// Final clamp for total spread
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float MaxCrosshairSpread = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float MaxTrackedAirSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float InAirSpreadInterpSpeed = 8.0f;

public:
	
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }		// Return CameraBoom
	FORCEINLINE UCameraComponent* GetPlayerCamera() const { return PlayerCamera; }		// Return PlayerCamera
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }							// Return if currently aiming
	FORCEINLINE float GetCrosshairSpread() const { return CurrentCrosshairSpread; }		// Return the current value of the crosshair spread

};
