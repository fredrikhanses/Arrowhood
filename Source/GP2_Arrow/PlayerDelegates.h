// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerDelegates.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDelegates);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP2_ARROW_API UPlayerDelegates : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	
	UPlayerDelegates();

	UPROPERTY(BlueprintAssignable)
	FPlayerDelegates FreezeStarted;

	UPROPERTY(BlueprintAssignable)
	FPlayerDelegates FreezeEnded;

	UPROPERTY(BlueprintAssignable)
	FPlayerDelegates LungeStarted;

	UPROPERTY(BlueprintAssignable)
	FPlayerDelegates LungeEnded;

	UPROPERTY(BlueprintAssignable)
	FPlayerDelegates StunStarted;

	UPROPERTY(BlueprintAssignable)
	FPlayerDelegates StunEnded;
		
};
