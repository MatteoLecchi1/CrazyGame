// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"
#include "Debuff.generated.h"

/**
 * 
 */

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class CRAZY_API UDebuff : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	int Counter = 1;
	UPROPERTY(BlueprintReadWrite)
	AGameplayCharacter* DebuffedCharacter;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnBeginTurnWithDebuff(AGridManagerActor* Grid);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnEndTurnWithDebuff(AGridManagerActor* Grid);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float OnDebuffApplyed(AGridManagerActor* Grid, AGameplayCharacter* InstigatingCharacter);
};
