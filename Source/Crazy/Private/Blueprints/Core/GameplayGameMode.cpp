// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Core/GameplayGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprints/Player/PlayerPawn.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"
#include "Blueprints/AI/EnemyPawn.h"

void AGameplayGameMode::GiveTurnToPlayer()
{
	for (auto character : PlayerCharacters) {
		character->OnTurnStart();
	}

	PlayerPawn->OnTurnStart();
}
void AGameplayGameMode::GiveTurnToEnemy()
{
	for (auto character : BanditCharacters) {
		character->OnTurnStart();
	}

	EnemyPawn->OnTurnStart();
}