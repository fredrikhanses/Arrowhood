// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GP2_ArrowHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

AGP2_ArrowHUD::AGP2_ArrowHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/T_CustomCrosshair"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> HitMarkerTexObj(TEXT("/Game/FirstPerson/Textures/T_CrosshairAndMarker"));
	CrosshairTex = CrosshairTexObj.Object;
	HitMarkerTex = HitMarkerTexObj.Object;
	NormalCrosshairTex = CrosshairTex;
}


void AGP2_ArrowHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X - 20),
										   (Center.Y));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
}

void AGP2_ArrowHUD::OnHit()
{
	CrosshairTex = HitMarkerTex;
	GetWorld()->GetTimerManager().SetTimer(HitMarkerTimer, this, &AGP2_ArrowHUD::ResetCrosshair, TimerLength, false);
}

void AGP2_ArrowHUD::ResetCrosshair()
{
	CrosshairTex = NormalCrosshairTex;
	GetWorld()->GetTimerManager().ClearTimer(HitMarkerTimer);
}
