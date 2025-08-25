// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Core/SkillManagerActor.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"
#include "Chaos/DebugDrawQueue.h"

// Sets default values
ASkillManagerActor::ASkillManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASkillManagerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkillManagerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkillManagerActor::ManageSkill(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, AGameplayCharacter* SkillUser)
{
	TArray<FInt32Vector2> AOETiles = FindSkillAOE(skillUsed, targetedTile, SkillUser);
	TArray<AGameplayCharacter*> Targets = FindSkillTargets(skillUsed, AOETiles, targetedTile, SkillUser);
	PlaySkill(skillUsed, Targets);
}

TArray<FInt32Vector2> ASkillManagerActor::FindSkillAOE(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, AGameplayCharacter* SkillUser)
{
	TArray<FInt32Vector2> AOETiles;

	switch (skillUsed->AOEtype)
	{
	case AOEType::SINGLETILE:
		AOETiles.Add(targetedTile);
		break;
	case AOEType::AOE:
		AOETiles = FindAOESkillAOE(skillUsed, targetedTile, SkillUser);
		break;
	case AOEType::DIRECTIONALAOE:
		AOETiles = FindDIRECTIONALAOESkillAOE(skillUsed, targetedTile, SkillUser);
		targetedTile = SkillUser->CurrentTile;
		break;
	default:
		break;
	}
	for (int i = 0; i< AOETiles.Num(); i++)
	{
		if (Grid->GetTileDefinition(AOETiles[i]))
		{
			if(Grid->CheckForObstruction(targetedTile, AOETiles[i]).bBlockingHit)
			{
				AOETiles.RemoveAt(i);
				i--;
				continue;
			}
		}
		else
		{
			AOETiles.RemoveAt(i);
			i--;
		}
	}
	return AOETiles;
}
TArray<FInt32Vector2> ASkillManagerActor::FindAOESkillAOE(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, AGameplayCharacter* SkillUser)
{
	TArray<FInt32Vector2> AOETiles = skillUsed->AOETiles;
	for (int i = 0; i <AOETiles.Num(); i++) 
	{
		AOETiles[i] += targetedTile;
	}
	return AOETiles;
}

TArray<FInt32Vector2> ASkillManagerActor::FindDIRECTIONALAOESkillAOE(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, AGameplayCharacter* SkillUser)
{
	FInt32Vector2 relativeTargetedTile = SkillUser->CurrentTile - targetedTile;
	int TargetAreaDirection = 0;
	if (abs(relativeTargetedTile.X) > abs(relativeTargetedTile.Y))
	{
		if (relativeTargetedTile.X > 0)
			TargetAreaDirection = 0;
		else
			TargetAreaDirection = 1;
	}
	else
	{
		if (relativeTargetedTile.Y > 0)
			TargetAreaDirection = 2;
		else
			TargetAreaDirection = 3;
	}

	TArray<FInt32Vector2> AOETiles = skillUsed->AOETiles;
	int X = 0;
	for (int i = 0; i < AOETiles.Num(); i++)
	{
		switch (TargetAreaDirection)
		{
		case 0:
			AOETiles[i].X *= -1;
			break;
		case 1:
			break;
		case 2:
			X = AOETiles[i].X;
			AOETiles[i].X = AOETiles[i].Y;
			AOETiles[i].Y = X * -1;
			break;
		case 3:
			X = AOETiles[i].X;
			AOETiles[i].X = AOETiles[i].Y;
			AOETiles[i].Y = X;
			break;
		default:
			break;
		}
		AOETiles[i] += SkillUser->CurrentTile;
	}
	return AOETiles;
}

TArray<AGameplayCharacter*> ASkillManagerActor::FindSkillTargets(FSkillDefinition* skillUsed, TArray<FInt32Vector2> targetedTiles, FInt32Vector2 targetedTile, AGameplayCharacter* SkillUser)
{
	TArray<AGameplayCharacter*> Targets;
	for (auto tile : targetedTiles)
	{
		auto tileDefinition = Grid->GetTileDefinition(tile);
		if (!tileDefinition)
			continue;
		if (!tileDefinition->Occupant)
			continue;
		if (AGameplayCharacter* target = Cast<AGameplayCharacter>(tileDefinition->Occupant))
		{
			Targets.Add(target);
		}
	}
	return Targets;
}

void ASkillManagerActor::PlaySkill(FSkillDefinition* skillUsed, TArray<AGameplayCharacter*> Targets)
{
	for (AGameplayCharacter* Target : Targets)
	{
		for (auto damageInstance : skillUsed->Damage)
		{
			Target->MyTakeDamage(damageInstance.DamageAmount, damageInstance.DamageElement);
		}
	}
}



