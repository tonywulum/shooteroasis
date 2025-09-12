// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/* Camera boom to place camera away from the actor*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraSet, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/* Camera that will be used by the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraSet, meta = (AllowPrivateAccess = "true"))
	class  UCameraComponent* PlayerCamera;

public:
	
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }		// Return CameraBoom
	FORCEINLINE UCameraComponent* GetPlayerCamera() const { return PlayerCamera; }		// Return PlayerCamera


};
