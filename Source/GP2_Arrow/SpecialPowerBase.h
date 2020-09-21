// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpecialPowerBase.generated.h"

UCLASS(Abstract)
class GP2_ARROW_API ASpecialPowerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpecialPowerBase();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Player)
	class AGP2_ArrowCharacter* Player;

	void SetPlayer(AGP2_ArrowCharacter* InPlayer);

};
