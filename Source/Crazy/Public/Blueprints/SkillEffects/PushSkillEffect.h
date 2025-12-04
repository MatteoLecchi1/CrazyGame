// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/TileDefinition.h"
#include "Blueprints/Grid/GridManagerActor.h"
#include "Blueprints/Core/SkillEffect.h"
#include "PushSkillEffect.generated.h"

/**
 * 
 */
UCLASS()
class CRAZY_API UPushSkillEffect : public USkillEffect
{
	GENERATED_BODY()

public:
	virtual float PlaySkillEffect_Implementation(AGameplayCharacter* SkillUser, FIntVector2 targetedTile, FSkillDefinition skillUsed, const TArray<AGameplayCharacter*>& Targets) override;
private:
	float PushSingleTarget(AGameplayCharacter* SkillUser, FIntVector2 targetedTile, FSkillDefinition skillUsed, const TArray<AGameplayCharacter*>& Targets);
	float PushAOE(AGameplayCharacter* SkillUser, FIntVector2 targetedTile, FSkillDefinition skillUsed, const TArray<AGameplayCharacter*>& Targets);
	float PushDirectionalAOE(AGameplayCharacter* SkillUser, FIntVector2 targetedTile, FSkillDefinition skillUsed, const TArray<AGameplayCharacter*>& Targets);
};
