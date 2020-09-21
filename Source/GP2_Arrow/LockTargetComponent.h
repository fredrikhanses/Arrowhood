// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "LockTargetComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargettingStatus, AGP2_ArrowCharacter*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopTargetting);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class GP2_ARROW_API ULockTargetComponent2 : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULockTargetComponent2();

protected:



public:	

	UPROPERTY(VisibleAnywhere)
	class AGP2_ArrowCharacter* Owner;

	UPROPERTY(VisibleAnywhere)
	FTimerHandle LockTargetHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targetting)
	TArray<AActor*> HitMarkers;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targetting)
	TArray<AActor*> HitMarkerPlayerPtr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Targetting)
	AActor* TargettedHitMarker;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Targetting)
	int TargettedHitMarkerIndex;

	UPROPERTY(VisibleAnywhere)
	TArray<AGP2_ArrowCharacter*> EnemyPlayers;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<AActor*> TargetableTargets;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	AActor* TargettedTarget;

	UFUNCTION(BlueprintCallable)
	void RemoveTargetableTarget(AActor* Target);

	UFUNCTION(BlueprintCallable)
	void AddTargetableTarget(AActor* Target);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Targetting)
	FVector HitMarkerOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Targetting)
	int NumberOfEnemyPlayers = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Targetting)
	uint32 bUseMarkers : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Targetting)
	TSubclassOf<AActor> TargetableType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Targetting)
	float UpdateRate = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Targetting)
	float MaxDegrees = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Targetting)
	float MaxTargetingDistance = 10000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Targetting)
	float MinTargetingDistance = 250;
	UFUNCTION()
	void LockTarget();

	UFUNCTION()
	void LockTargetFunc();

	UFUNCTION()
	void UnLockTarget();

	bool bToggle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Targetting)
	AGP2_ArrowCharacter* TargetedCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Targetting)
	float DistanceToTarget = 0;


	UFUNCTION(BlueprintCallable)
	void AddTargetableEnemy(AGP2_ArrowCharacter* AddTarget);
	

public:
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = LockTarget)
	float Sensitivity = 18.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = LockTarget)
	float Max = 35.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = LockTarget)
	float MaxCameraRotationPerSecond = 5.0f;
		
	FVector Offset;

	UPROPERTY(BlueprintAssignable, Category = Targetting)
	FTargettingStatus StartedTargetingEnemy;

	UPROPERTY(BlueprintAssignable, Category = Targetting)
	FTargettingStatus CantTargetEnemy;

	UPROPERTY(BlueprintAssignable, Category = Targetting)
	FTargettingStatus CanTargetEnemy;

	UPROPERTY(BlueprintAssignable, Category = Targetting)
	FStopTargetting StoppedTargetingEnemy;

	UFUNCTION()
	void TestTargetting();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targetting)
	TArray<AGP2_ArrowCharacter*> TargetableEnemies;

	UFUNCTION()
	void FindTarget();


	void BeginPlay() override;

};
