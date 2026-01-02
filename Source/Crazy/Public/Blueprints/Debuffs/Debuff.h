// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"
#include "Debuff.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ReApplicationResponse : uint8
{
	ADD = 0		UMETA(DisplayName = "ADD BASE COUNTER"),
	RESET = 1	UMETA(DisplayName = "RESET TO BASE COUNTER"),
	IGNORE = 2	UMETA(DisplayName = "DO NOTHING"),
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class CRAZY_API UDebuff : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	int MaxCounter = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	int BaseCounter = 3;
	UPROPERTY(BlueprintReadWrite, Category = "Values")
	int Counter = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	int BaseReward = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	ReApplicationResponse reApplicationResponse = ReApplicationResponse::RESET;
	UPROPERTY(BlueprintReadWrite)
	AGameplayCharacter* DebuffedCharacter;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnBeginTurnWithDebuff(AGridManagerActor* Grid);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnEndTurnWithDebuff(AGridManagerActor* Grid);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float OnDebuffApplyed(AGridManagerActor* Grid, AGameplayCharacter* InstigatingCharacter);
};
