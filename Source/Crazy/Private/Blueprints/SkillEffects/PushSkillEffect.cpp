// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/SkillEffects/PushSkillEffect.h"

float UPushSkillEffect::PlaySkillEffect_Implementation(AGameplayCharacter* SkillUser, FIntVector2 targetedTile, FSkillDefinition skillUsed, const TArray<AGameplayCharacter*>& Targets)
{
	float reward = 0;
	switch (skillUsed.AOEtype)
	{
	case AOEType::SINGLETILE:
		reward = PushSingleTarget(SkillUser, targetedTile, skillUsed, Targets);
		break;
	case AOEType::AOE:
		reward = PushAOE(SkillUser, targetedTile, skillUsed, Targets);
		break;
	case AOEType::DIRECTIONALAOE:
		reward = PushDirectionalAOE(SkillUser, targetedTile, skillUsed, Targets);
		break;
	default:
		break;
	}
	return reward;
}

float UPushSkillEffect::PushSingleTarget(AGameplayCharacter* SkillUser, FIntVector2 targetedTile, FSkillDefinition skillUsed, const TArray<AGameplayCharacter*>& Targets)
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

float UPushSkillEffect::PushDirectionalAOE(AGameplayCharacter* SkillUser, FIntVector2 targetedTile, FSkillDefinition skillUsed, const TArray<AGameplayCharacter*>& Targets)
{
	return 0;
}
