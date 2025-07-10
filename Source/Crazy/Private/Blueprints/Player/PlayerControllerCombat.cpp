// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Player/PlayerControllerCombat.h"
#include "Blueprints/Player/PlayerPawn.h"

void APlayerControllerCombat::BeginPlay()
{
	Super::BeginPlay();
	SetShowMouseCursor(true);
	PlayerPawn = Cast<APlayerPawn>(GetPawn());
}