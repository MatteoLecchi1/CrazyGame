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

	auto characters = EnemyTurnOrder;
	int i = 1;

	for (auto character : characters)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, FName("PlayCharacterTurn"), character);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, Delegate, DelayBetweenTurns * i + 0.01f, false);
		i++;
	}

	FTimerHandle TimerHandle;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, FName("EndTurn"));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, Delegate, DelayBetweenTurns * i + 0.01f, false);
}

void AEnemyPawn::PlayCharacterTurn(AGameplayCharacter* character)
{
	auto targetCharacter = Grid->FindClosestCharacter(character, GameMode->PlayerCharacters);

	if (!targetCharacter)
		return;

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
	FSkillDefinition* skill = &character->Skills[skillToUse];
	character->UseSkillAsCharacter(skill, targetCharacter->CurrentTile);
}

void AEnemyPawn::EndTurn()
{
	GameMode->GiveTurnToPlayer();
}