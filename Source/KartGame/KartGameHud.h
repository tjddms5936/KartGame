// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "KartGameHud.generated.h"


UCLASS(config = Game)
class AKartGameHud : public AHUD
{
	GENERATED_BODY()

public:
	AKartGameHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface
};
