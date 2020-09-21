// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HookShotComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHookShotUpdate);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class GP2_ARROW_API UHookShotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHookShotComponent();
	

	UPROPERTY(BlueprintAssignable)
		FHookShotUpdate HookShotSuccess;
	
	UPROPERTY(BlueprintAssignable)
		FHookShotUpdate HookShotOutOfRange;
	UPROPERTY(BlueprintAssignable)
		FHookShotUpdate HookShotStillOnCooldown;

	UPROPERTY(BlueprintAssignable)
		FHookShotUpdate HookShotUpdate;

	UPROPERTY(BlueprintAssignable)
		FHookShotUpdate HookShotStarted;

	UPROPERTY(BlueprintAssignable)
		FHookShotUpdate HookShotEnded;



	UPROPERTY(EditAnywhere, Category = "HookShot")
	uint32 bHasHookShotPosition : 1;

	UPROPERTY(EditAnywhere, Category = "HookShot")
		float HookShotDistance = 5000.0f;
	UPROPERTY(EditAnywhere, Category = "HookShot")
	float HookShotMinDistance = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HookShot")
	float HookShotForce = 2050.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "HookShot")
	float HookShotCooldown = 4.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "HookShot")
	float HookShotAccu = 4.0f;


	FVector HookShotStartPosition;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HookShot")
		FVector HookShotEndPosition;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = HookShot)
		FVector HookShotLaunchDir;
	float HookShotForceAccu;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Hookshot")
		uint32 bStartedHookShot : 1;
};
