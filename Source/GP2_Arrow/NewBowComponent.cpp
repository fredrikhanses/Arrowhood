// Fill out your copyright notice in the Description page of Project Settings.
#include "NewBowComponent.h"
#include "Engine/World.h"


// Sets default values for this component's properties
UNewBowComponent::UNewBowComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	CanShootAgain = true;
	FireRateCheck = FireRate;
	ArrowCharge = 4;
	MaxArrowCharge = ArrowCharge;
	i = ArrowCharge;
	// ...
}


// Called when the game starts
void UNewBowComponent::BeginPlay()
{
	Super::BeginPlay();
	ArrowArray.Init(ReChargerTimer, ArrowCharge);
}


// Called every frame
void UNewBowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	
	if (ArrowCharge < 5)
	{
		ChargeShot();
	}
	else {
		internalChargeTimerCheck = 0.0f;
	}
	// ...
}

void UNewBowComponent::ChargeShot()
{
	
	if (internalChargeTimerCheck < internalChargeTimer)
	{
		internalChargeTimerCheck += GetWorld()->GetDeltaSeconds();
	}else if (internalChargeTimerCheck >= internalChargeTimer)
	{
		ArrowCharge++;
		internalChargeTimerCheck = 0;
	}
}

void UNewBowComponent::SetChargeToZero()
{
	
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No More Chargers"));
		return;
}

void UNewBowComponent::FireRateCalculatorThing()
{
	if (FireRate >= FireRateCheck)
	{
		CanShootAgain = true;
	}
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Poop"));
		FireRate += 0.01f;
	return;
}

