// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/AI/EnemyPawn.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"
#include "Blueprints/Core/SkillManagerActor.h"
#include "Chaos/DebugDrawQueue.h"

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
	int FavoredSkillIndex  = -1;
	FIntVector2 FavoredTileToWalkTo = FIntVector2(-1, -1);
	FIntVector2 FavoredTileToTarget = FIntVector2(-1, -1);
	int currentSkillIndex = 0;
	float FavoredSkillReward = MinimumAcceptableReward;

	for (auto PossibleTarget : GameMode->PlayerCharacters) 
	{
		float currentSkillReward = MinimumAcceptableReward;
		FIntVector2 TileToWalkTo = FIntVector2(-1, -1);
		FIntVector2 TileToTarget = FIntVector2(-1, -1);

		int skillIndex = 0;

		for (auto skill : character->Skills)
		{
			if (skill.APCost > character->CurrentAP)
				continue;
			if (skill.CurrentCooldown > 0)
				continue;

			float offsetReward = -1;
			FTileAndReward tile;
			switch (skill.AOEtype)
			{
			case AOEType::SINGLETILE:
				tile = FindFavoredTileToUseSkill(character, skill, PossibleTarget->CurrentTile);

				if (tile.Tile.X == -1)
					continue;

				currentSkillReward = tile.Reward;
				currentSkillReward += GameMode->SkillManagerActor->CheckManageSkill(&skill, PossibleTarget->CurrentTile, character);
				TileToWalkTo = tile.Tile;
				TileToTarget = PossibleTarget->CurrentTile;
				currentSkillIndex = skillIndex;

				break;
			case AOEType::AOE:
				for(auto offset : skill.AOETiles)
				{
					if (!Grid->GetTileDefinition(PossibleTarget->CurrentTile + offset))
						continue;
					tile = FindFavoredTileToUseSkill(character, skill, PossibleTarget->CurrentTile + offset);

					if(tile.Tile.X == -1)
						continue;

					offsetReward = GameMode->SkillManagerActor->CheckManageSkill(&skill, PossibleTarget->CurrentTile + offset, character);

					if (offsetReward > currentSkillReward)
					{
						currentSkillReward = tile.Reward;
						currentSkillReward = offsetReward; 
						TileToWalkTo = tile.Tile;
						TileToTarget = PossibleTarget->CurrentTile + offset;
						currentSkillIndex = skillIndex;
					}
				}
				break;
			case AOEType::DIRECTIONALAOE:
				for (auto offset : skill.AOETiles)
				{
					FIntVector2 offsetTile = PossibleTarget->CurrentTile + offset;
					if (!Grid->GetTileDefinition(offsetTile))
						continue;
					if (character->CheckWalkToTileAPCost(offsetTile) + skill.APCost >= character->CurrentAP)
						continue;

					offsetReward = GameMode->SkillManagerActor->CheckManageSkill(&skill, PossibleTarget->CurrentTile + offset, character);

					if (offsetReward > currentSkillReward)
					{
						currentSkillReward = offsetReward;
						TileToWalkTo = offsetTile;
						TileToTarget = PossibleTarget->CurrentTile;
						currentSkillIndex = skillIndex;
					}
				}
				break;
			default:
				break;
			}
			FavoredSkillReward += skill.APCost * character->APWeight;
			skillIndex++;
		}

		if (FavoredSkillReward < currentSkillReward) 
		{
			FavoredSkillReward = currentSkillReward;
			FavoredTileToTarget = TileToTarget;
			FavoredTileToWalkTo = TileToWalkTo;
			FavoredSkillIndex = currentSkillIndex;
		}
	}
	if (FavoredSkillReward > MinimumAcceptableReward)
	{
		if(FavoredTileToTarget.X > -1)
		DrawDebugSphere(GetWorld(), Grid->GetTileDefinition(FavoredTileToTarget)->Location, 50.f, 32, FColor::Red, false, DelayBetweenTurns);

		character->WalkToTileAsCharacter(FavoredTileToWalkTo);
		character->UseSkillAsCharacter(&character->Skills[FavoredSkillIndex], FavoredTileToTarget);
		FString str = FString::FromInt(character->Skills[FavoredSkillIndex].CurrentCooldown);
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, character->Skills[FavoredSkillIndex].SkillDysplayName.ToString() + ", now on cooldown for " + str);
	}
}
FTileAndReward AEnemyPawn::FindFavoredTileToUseSkill(class AGameplayCharacter* character, FSkillDefinition skill, FInt32Vector2 TargetTile)
{
	int maxRange = skill.MaxRange;
	int minRange = skill.MinRange;

	FTileAndReward ReturnValue;
	ReturnValue.Tile = FInt32Vector2(-1, -1); 
	ReturnValue.Reward = -30000;

	float CurrentReward = -1;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int X = -maxRange; X <= maxRange; X++)
	{
		for (int Y = -maxRange; Y <= maxRange; Y++)
		{
			FIntVector2 currentTile = TargetTile + FIntVector2(X, Y);
			int distance = Grid->CalculateDistance(TargetTile, currentTile);

			if (!(distance >= minRange && distance <= maxRange))
				continue;

			if (auto currentTileDefinition = Grid->GetTileDefinition(currentTile))
			{
				if (Grid->CheckForObstruction(TargetTile, currentTile).bBlockingHit)
					continue;

				FIntVector2 walkCost = character->CheckWalkToTileAPCostAndRemaningWalk(currentTile);
				int ApCost = walkCost.X;
				int remaningWalkDistance = walkCost.Y;

				if (ApCost + skill.APCost > character->CurrentAP)
					continue;

				int favoredDistanceOffset = abs(character->FavoredDistanceFromTarget - distance);
				CurrentReward = favoredDistanceOffset * character->FavoredDistanceMultiplierPerTile;
				CurrentReward += ApCost * character->APWeight;
				CurrentReward += remaningWalkDistance * character->RemaningWalkDistanceWeight;
				if(CurrentReward > ReturnValue.Reward)
				{
					ReturnValue.Tile = currentTile; 
					ReturnValue.Reward = CurrentReward;
				}
			}
		}
	}
	return ReturnValue;
}
void AEnemyPawn::EndTurn()
{
	GameMode->GiveTurnToPlayer();
}