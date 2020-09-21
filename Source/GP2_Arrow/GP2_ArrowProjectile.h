// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <GameFramework/Character.h>
#include "GP2_ArrowProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FArrowStuck);

UCLASS(config=Game)
class AGP2_ArrowProjectile : public AActor
{
	GENERATED_BODY()
public:
	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	class AGP2_ArrowCharacter* Player;

public:
	AGP2_ArrowProjectile();

	UPROPERTY(BlueprintAssignable)
	FArrowStuck ArrowStuckOnSpawn;

	/** called when projectile hits something */
	/*UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);*/

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool HasHitCPP;
};

