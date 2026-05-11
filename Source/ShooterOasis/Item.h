// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;

UCLASS()
class SHOOTEROASIS_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> CollisionBox;

public:	

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh.Get(); }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox.Get(); }
};
