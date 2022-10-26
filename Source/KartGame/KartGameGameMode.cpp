// Copyright Epic Games, Inc. All Rights Reserved.

#include "KartGameGameMode.h"
#include "KartGamePawn.h"
#include "KartGameHud.h"

AKartGameGameMode::AKartGameGameMode()
{
	DefaultPawnClass = AKartGamePawn::StaticClass();
	HUDClass = AKartGameHud::StaticClass();
}
