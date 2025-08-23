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

		auto targetsNeighbors = Grid->GetValidTileNeighbors(targetCharacter->CurrentTile);

		auto pathToTargetCharacter = Grid->FindPath(character->CurrentTile, targetsNeighbors[0]);

		if (distance > character->Skills[0].MaxRange)
		{
			FInt32Vector2 targetWalkTile = FInt32Vector2(-1, -1);
			for (auto tile : pathToTargetCharacter)
			{
				if (Grid->CalculateDistance(tile, targetCharacter->CurrentTile) > character->Skills[0].MaxRange)
					continue;
				if (Grid->CheckForObstruction(tile, targetCharacter->CurrentTile).bBlockingHit)
					continue;
				targetWalkTile = tile;
				break;
			}
			character->WalkToTileAsCharacter(targetWalkTile);
		}

		int skillToUse = -1;

		if (!(distance < character->Skills[0].MinRange))
		{
			skillToUse = 0;
		}
		else
		{
			skillToUse = 1;
		}

		if (skillToUse < 0)
			return;

		character->UseSkillAsCharacter(character->Skills[skillToUse], targetCharacter->CurrentTile);
	}
	EndTurn();
}

void AEnemyPawn::EndTurn()
{
	GameMode->GiveTurnToPlayer();
}