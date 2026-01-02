// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/Debuffs/Debuff.h"
#include "DOTDebuff.generated.h"

/**
 * 
 */
UCLASS()
class CRAZY_API UDOTDebuff : public UDebuff
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "DOTValues")
	int Damage = 10;
	UPROPERTY(EditAnywhere, Category = "DOTValues")
	DamageElements DamageElement = DamageElements::MAGIC;
	UPROPERTY(EditAnywhere, Category = "DOTValues")
	int BaseApplicationReward = 10;

	void OnBeginTurnWithDebuff_Implementation(AGridManagerActor* Grid) override;
	void OnEndTurnWithDebuff_Implementation(AGridManagerActor* Grid) override;
	float OnDebuffApplyed_Implementation(AGridManagerActor* Grid, AGameplayCharacter* InstigatingCharacter) override;
};
