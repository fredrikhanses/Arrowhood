// Fill out your copyright notice in the Description page of Project Settings.


#include "SpecialPowerBase.h"

ASpecialPowerBase::ASpecialPowerBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpecialPowerBase::SetPlayer(AGP2_ArrowCharacter* InPlayer)
{
	Player = InPlayer;
}
