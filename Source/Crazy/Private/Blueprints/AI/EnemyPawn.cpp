// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/AI/EnemyPawn.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"

void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

	Initialize();
}
void AEnemyPawn::Initialize()
{
	Super::Initialize();

	GameMode->EnemyPawn = this;
}

// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AEnemyPawn::OnTurnStart()
{
	Super::OnTurnStart();

	auto characters = GameMode->BanditCharacters;

	for (auto character : characters) 
	{
		
		auto targetCharacter = Grid->FindClosestCharacter(character,GameMode->PlayerCharacters);

		if (!targetCharacter)
			continue;

		auto distance = Grid->CalculateDistance(character->CurrentTile, targetCharacter->CurrentTile);

		if (distance > 1)
		{
			auto targetsNeighbors = Grid->GetValidTileNeighbors(targetCharacter->CurrentTile);
			character->WalkToTile(targetsNeighbors[0], this);
		}

		character->UseSkill(character->Skills[0], targetCharacter->CurrentTile, this);
	}
	EndTurn();
}

void AEnemyPawn::EndTurn()
{
	GameMode->GiveTurnToPlayer();
}