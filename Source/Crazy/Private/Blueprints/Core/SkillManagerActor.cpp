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

void ASkillManagerActor::ManageSkill(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, FInt32Vector2 StartTile, AGameplayCharacter* SkillUser)
{
	TArray<std::tuple<FInt32Vector2, FInt32Vector2>> AOETiles = FindSkillAOE(skillUsed, targetedTile, StartTile);
	TArray<std::tuple<AGameplayCharacter*, FInt32Vector2>> Targets = FindSkillTargets(skillUsed, AOETiles, targetedTile, SkillUser);
	PlaySkill(skillUsed, Targets);
}

float ASkillManagerActor::CheckManageSkill(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, FInt32Vector2 StartTile, AGameplayCharacter* SkillUser)
{
	TArray<std::tuple<FInt32Vector2, FInt32Vector2>> AOETiles = FindSkillAOE(skillUsed, targetedTile, StartTile);
	TArray<std::tuple<AGameplayCharacter*, FInt32Vector2>> Targets = FindSkillTargets(skillUsed, AOETiles, targetedTile, SkillUser);
	return CheckPlaySkill(skillUsed,Targets, SkillUser);
}

TArray<std::tuple<FInt32Vector2, FInt32Vector2>> ASkillManagerActor::FindSkillAOE(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, FInt32Vector2 StartTile)
{
	TArray<std::tuple<FInt32Vector2, FInt32Vector2>> AOETiles;

	switch (skillUsed->AOEtype)
	{
	case AOEType::SINGLETILE:

		AOETiles = FindSINGLETILESkillAOE(skillUsed, StartTile, targetedTile);
		break;
	case AOEType::AOE:
		AOETiles = FindAOESkillAOE(skillUsed, targetedTile);
		break;
	case AOEType::DIRECTIONALAOE:
		AOETiles = FindDIRECTIONALAOESkillAOE(skillUsed, targetedTile, StartTile);
		targetedTile = StartTile;
		break;
	default:
		break;
	}
	for (int i = 0; i< AOETiles.Num(); i++)
	{
		if (Grid->GetTileDefinition(std::get<0>(AOETiles[i])))
		{
			if(Grid->CheckForObstruction(targetedTile, std::get<0>(AOETiles[i])).bBlockingHit)
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


TArray<std::tuple<FInt32Vector2, FInt32Vector2>> ASkillManagerActor::FindSINGLETILESkillAOE(FSkillDefinition* skillUsed, FInt32Vector2 StartTile, FInt32Vector2 targetedTile)
{
	TArray<std::tuple<FInt32Vector2, FInt32Vector2>> AOETiles;
	
	FVector endLocation = Grid->GetTileDefinition(targetedTile)->Location;
	FVector startLocation = Grid->GetTileDefinition(StartTile)->Location;
	FVector pushDirection = endLocation - startLocation;
	pushDirection.Normalize();
	auto pushAngle = FVector::DotProduct(pushDirection, FVector(1, 0, 0));

	std::tuple<FInt32Vector2, FInt32Vector2> Tile = std::tuple<FInt32Vector2, FInt32Vector2>(targetedTile,targetedTile);
	AOETiles.Add(Tile);

	return AOETiles;
}

TArray<std::tuple<FInt32Vector2, FInt32Vector2>> ASkillManagerActor::FindAOESkillAOE(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile)
{
	TArray<std::tuple<FInt32Vector2, FInt32Vector2>> AOETiles;
	for (int i = 0; i < skillUsed->AOETiles.Num(); i++)
	{
		FInt32Vector2 currentTargetedTile;
		currentTargetedTile = skillUsed->AOETiles[i];
		currentTargetedTile += targetedTile;

		FInt32Vector2 currentPushTile;
		currentPushTile = skillUsed->AOEPushDirections[i];
		currentPushTile += targetedTile;

		AOETiles.Add(std::tuple<FInt32Vector2, FInt32Vector2>(currentTargetedTile, currentPushTile));
	}
	return AOETiles;
}

TArray<std::tuple<FInt32Vector2, FInt32Vector2>> ASkillManagerActor::FindDIRECTIONALAOESkillAOE(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, FInt32Vector2 StartTile)
{
	FInt32Vector2 relativeTargetedTile = StartTile - targetedTile;
	int TargetAreaDirection = 0;
	if (abs(relativeTargetedTile.X) > abs(relativeTargetedTile.Y))
	{
		if (relativeTargetedTile.X > 0)
			TargetAreaDirection = 2;
		else
			TargetAreaDirection = 0;
	}
	else
	{
		if (relativeTargetedTile.Y > 0)
			TargetAreaDirection = 1;
		else
			TargetAreaDirection = 3;
	}

	TArray<std::tuple<FInt32Vector2, FInt32Vector2>> AOETiles;
	
	for (int i = 0; i < skillUsed->AOETiles.Num(); i++)
	{
		FInt32Vector2 currentTargetedTile;
		currentTargetedTile = skillUsed->AOETiles[i];
		currentTargetedTile = Grid->RotateOffset(currentTargetedTile, TargetAreaDirection);
		currentTargetedTile += StartTile;

		FInt32Vector2 currentPushTile;
		currentPushTile = skillUsed->AOEPushDirections[i];
		currentPushTile = Grid->RotateOffset(currentPushTile, TargetAreaDirection);
		currentPushTile += StartTile;

		AOETiles.Add(std::tuple<FInt32Vector2, FInt32Vector2>(currentTargetedTile, currentPushTile));
	}
	return AOETiles;
}

TArray<std::tuple<AGameplayCharacter*, FInt32Vector2>> ASkillManagerActor::FindSkillTargets(FSkillDefinition* skillUsed, TArray<std::tuple<FInt32Vector2, FInt32Vector2>> targetedTiles, FInt32Vector2 targetedTile, AGameplayCharacter* SkillUser)
{
	TArray<std::tuple<AGameplayCharacter*, FInt32Vector2>> Targets;
	for (auto tile : targetedTiles)
	{
		auto tileDefinition = Grid->GetTileDefinition(std::get<0>(tile));
		if (!tileDefinition)
			continue;
		if (!tileDefinition->Occupant)
			continue;
		if (AGameplayCharacter* target = Cast<AGameplayCharacter>(tileDefinition->Occupant))
		{
			Targets.Add(std::tuple<AGameplayCharacter*, FInt32Vector2>(target,std::get<1>(tile)));
		}
	}
	return Targets;
}

void ASkillManagerActor::PlaySkill(FSkillDefinition* skillUsed, TArray<std::tuple<AGameplayCharacter*, FInt32Vector2>> Targets)
{
	for (auto Target : Targets)
	{
		for (auto damageInstance : skillUsed->Damage)
		{
			std::get<0>(Target)->MyTakeDamage(damageInstance.DamageAmount, damageInstance.DamageElement);
		}
		std::get<0>(Target)->PushToTile(std::get<1>(Target));
	}
}

float ASkillManagerActor::CheckPlaySkill(FSkillDefinition* skillUsed, TArray<std::tuple<AGameplayCharacter*, FInt32Vector2>> Targets, AGameplayCharacter* SkillUser)
{
	float reward = 0;
	for (auto Target : Targets)
	{
		if (std::get<0>(Target) == SkillUser)
			continue;
		for (auto damageInstance : skillUsed->Damage)
		{
			float damage = std::get<0>(Target)->CheckInflictedDamage(damageInstance.DamageAmount, damageInstance.DamageElement);
			float rewardWeight = 1;

			if (std::get<0>(Target)->Faction == SkillUser->Faction)
				rewardWeight = SkillUser->FriendlyDamageWeight;
			else
				rewardWeight = SkillUser->DamageWeight;

			reward += damage * rewardWeight;
		}
	}
	return reward;
}



