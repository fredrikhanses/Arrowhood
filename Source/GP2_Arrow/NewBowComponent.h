// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Engine.h>
#include "TimerManager.h"
#include "Components/ActorComponent.h"
#include "NewBowComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP2_ARROW_API UNewBowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow Properties")
		TArray<float> ArrowArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow Properties")
		int ArrowCharge;
		int i;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow Properties")
		int MaxArrowCharge;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow Properties")
		float ReChargerTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow Properties")
		float FireRate;
		float FireRateCheck;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow Properties")
		bool CanShootAgain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow Properties")
		float internalChargeTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow Properties")
		float internalChargeTimerCheck;

	

	// Sets default values for this component's properties
	UNewBowComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Bow Function")
		void ChargeShot();
	UFUNCTION(BlueprintCallable, Category = "Bow Function")
		void SetChargeToZero();
	UFUNCTION(BlueprintCallable, Category = "Bow Function")
		void FireRateCalculatorThing();
		
};
