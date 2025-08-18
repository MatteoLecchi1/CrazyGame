// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Structs/TileDefinition.h"
#include "Blueprints/Grid/GridManagerActor.h"
#include "GameplayGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CRAZY_API AGameplayGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class APlayerPawn* PlayerPawn;
	UPROPERTY()
	TArray<class AGameplayCharacter*> PlayerCharacters;
	UPROPERTY()
	class AEnemyPawn* EnemyPawn;
	UPROPERTY()
	TArray<class AGameplayCharacter*> BanditCharacters;
	UPROPERTY()
	TArray<class AGameplayCharacter*> MonsterCharacters;
	
	void GiveTurnToPlayer();
	void GiveTurnToEnemy();

};
