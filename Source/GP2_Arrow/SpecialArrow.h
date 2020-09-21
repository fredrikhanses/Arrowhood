// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpecialPowerBase.h"
#include <Engine.h>
#include "SpecialArrow.generated.h"

/*
UENUM(BlueprintType)
enum class SpecialArrowHeadTypes : uint8
{
	AT_BombArrow UMETA(DisplayName = "Bomb ArrowHead"),
	AT_IceArrow UMETA(DisplayName = "Ice ArrowHead"),
};
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GP2_ARROW_API USpecialArrow : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USpecialArrow();

	//Arrow Mechanics and shit
	//UPROPERTY(EditAnywhere, Category = "Special Arrow Properties")
	//	SpecialArrowHeadTypes ArrowHeadTypes;
	UPROPERTY(EditAnywhere, Category = "Special Arrow Properties")
		TSubclassOf<ASpecialPowerBase> ArrowAbility;
	UPROPERTY()
		FVector ActorLocation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Player)
	class AGP2_ArrowCharacter* Player;
	class AGP2_ArrowProjectile* Projectile;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable, Category = "Special Arrow Function")
		void ActivateArrow();
	UFUNCTION(BlueprintCallable, Category = "Special Arrow Function")
		void SpawnArrowPower();
};
