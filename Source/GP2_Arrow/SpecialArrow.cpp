// Fill out your copyright notice in the Description page of Project Settings.


#include "SpecialArrow.h"
#include "GP2_ArrowProjectile.h"
#include "Components/SphereComponent.h"

USpecialArrow::USpecialArrow()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void USpecialArrow::BeginPlay()
{
	Super::BeginPlay();
	Projectile = Cast<AGP2_ArrowProjectile>(GetOwner());

}

void USpecialArrow::ActivateArrow()
{
	if (ArrowAbility)
	{
		SpawnArrowPower();
	}
}

void USpecialArrow::SpawnArrowPower()
{
	ActorLocation = GetOwner()->GetActorLocation();
	FRotator TheRotation = GetOwner()->GetActorRotation();
	//TheRotation.Roll = 0.0f;
	//TheRotation.Pitch = 0.0f;
	//TheRotation.Yaw += 90.0f;

	FTransform Transform = FTransform( TheRotation, ActorLocation, FVector::OneVector);
	ASpecialPowerBase* SpecialPower = GetWorld()->SpawnActorDeferred<ASpecialPowerBase>(ArrowAbility, Transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	SpecialPower->SetPlayer(Projectile->Player);
	UGameplayStatics::FinishSpawningActor(SpecialPower, Transform);
}