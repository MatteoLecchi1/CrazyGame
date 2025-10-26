// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/Core/GameplayPawn.h"
#include "EnemyPawn.generated.h"

/**
 * 
 */
USTRUCT()
struct CRAZY_API FTileAndReward
 {
	 GENERATED_BODY()

	 UPROPERTY()
	 FInt32Vector2 Tile = FInt32Vector2(-1, -1);
	 UPROPERTY()
	 float Reward = -30000;
 };
UCLASS()
class CRAZY_API AEnemyPawn : public AGameplayPawn
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:

	UPROPERTY(EditAnywhere)
	TArray<class AGameplayCharacter*> EnemyTurnOrder;

	UPROPERTY(EditAnywhere)
	float DelayBetweenTurns;


	UPROPERTY(EditAnywhere, Category = "AI|Reward Weights")
	float MinimumAcceptableReward = -1;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Initialize() override;

	UFUNCTION()
	void PlayCharacterTurn(class AGameplayCharacter* character);
	FTileAndReward FindFavoredTileToUseSkill(class AGameplayCharacter* character, FSkillDefinition skill, FInt32Vector2 TargetTile);
	void OnTurnStart() override;
	UFUNCTION()
	void EndTurn();
};
