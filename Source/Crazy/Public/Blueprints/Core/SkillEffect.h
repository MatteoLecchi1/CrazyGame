// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"
#include "SkillEffect.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class CRAZY_API USkillEffect : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	float PlaySkillEffect(AGameplayCharacter* SkillUser, FSkillDefinition skillUsed, const TArray<AGameplayCharacter*> &Targets);
};
