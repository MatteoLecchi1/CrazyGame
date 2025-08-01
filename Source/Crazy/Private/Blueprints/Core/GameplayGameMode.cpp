// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Core/GameplayGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprints/Player/PlayerPawn.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"

void AGameplayGameMode::GiveTurnToPlayer()
{
	PlayerPawn->SetAP(PlayerPawn->CurrentAP + PlayerPawn->APregen);
	PlayerPawn->PlayersTurn = true;
	
	for (auto character : PlayersCharacters) {
		character->OnTurnStart();
	}
}
void AGameplayGameMode::GiveTurnToEnemy()
{
	GiveTurnToPlayer();
}