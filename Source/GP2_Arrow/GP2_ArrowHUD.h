// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GP2_ArrowHUD.generated.h"

UCLASS()
class AGP2_ArrowHUD : public AHUD
{
	GENERATED_BODY()

public:
	AGP2_ArrowHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	FTimerHandle HitMarkerTimer;

	UPROPERTY(EditAnywhere, Category = HitMarker)
	float TimerLength = 0.5f;

	UFUNCTION(BlueprintCallable)
	void OnHit();

	void ResetCrosshair();

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	class UTexture2D* HitMarkerTex;

	class UTexture2D* NormalCrosshairTex;
};

