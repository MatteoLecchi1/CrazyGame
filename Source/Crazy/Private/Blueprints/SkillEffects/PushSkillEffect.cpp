// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/SkillEffects/PushSkillEffect.h"

float UPushSkillEffect::PlaySkillEffect_Implementation(AGameplayCharacter* SkillUser, FIntVector2 targetedTile, FSkillDefinition skillUsed, const TArray<AGameplayCharacter*>& Targets, AGridManagerActor* Grid)
{
	float reward = 0;
	switch (skillUsed.AOEtype)
	{
	case AOEType::SINGLETILE:
		reward = PushSingleTarget(SkillUser, targetedTile, skillUsed, Targets, Grid);
		break;
	case AOEType::AOE:
		reward = PushAOE(SkillUser, targetedTile, skillUsed, Targets);
		break;
	case AOEType::DIRECTIONALAOE:
		reward = PushDirectionalAOE(SkillUser, targetedTile, skillUsed, Targets, Grid);
		break;
	default:
		break;
	}
	return reward;
}

float UPushSkillEffect::PushSingleTarget(AGameplayCharacter* SkillUser, FIntVector2 targetedTile, FSkillDefinition skillUsed, const TArray<AGameplayCharacter*>& Targets, AGridManagerActor* Grid)
{
	return 0;
}

float UPushSkillEffect::PushAOE(AGameplayCharacter* SkillUser, FIntVector2 targetedTile, FSkillDefinition skillUsed, const TArray<AGameplayCharacter*>& Targets)
{
	float reward = 0;
	for (auto target : Targets)
	{
		int index = skillUsed.AOEData.AOETiles.Find(target->CurrentTile - targetedTile);
		if (index == -1)
			continue;
		FIntVector2 pushTargetTile = skillUsed.AOEData.AOEPushDirections[index] + targetedTile;
		reward += target->PushToTile(pushTargetTile);
	}
	return reward;
}

float UPushSkillEffect::PushDirectionalAOE(AGameplayCharacter* SkillUser, FIntVector2 targetedTile, FSkillDefinition skillUsed, const TArray<AGameplayCharacter*>& Targets, AGridManagerActor* Grid)
{
	FIntVector2 relativeTargetedTile = SkillUser->CurrentTile - targetedTile;
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

	TArray<FIntVector2> AOETiles = skillUsed.AOEData.AOETiles;
	TArray<FIntVector2> AOEPushTiles = skillUsed.AOEData.AOEPushDirections;

	for (int i = 0; i < AOETiles.Num(); i++)
	{
		AOETiles[i] = Grid->RotateOffset(AOETiles[i], TargetAreaDirection);
		AOEPushTiles[i] = Grid->RotateOffset(AOEPushTiles[i], TargetAreaDirection);
	}
	float reward = 0;
	for (auto target : Targets)
	{
		int index = AOETiles.Find(target->CurrentTile - SkillUser->CurrentTile);
		if (index == -1)
			continue;
		FIntVector2 pushTargetTile = AOEPushTiles[index] + SkillUser->CurrentTile;
		reward += target->PushToTile(pushTargetTile);
	}
	return reward;
}
