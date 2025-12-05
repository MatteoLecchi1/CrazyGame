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

	AGameplayGameMode();

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


	UPROPERTY()
	TArray<class AGameplayCharacter*> DeathList;

	UPROPERTY()
	class ASkillManagerActor* SkillManagerActor;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASkillManagerActor> SkillManagerActorClass;

	UPROPERTY()
	int Tick1Timer = 0;

	virtual void Tick(float DeltaTime) override;
	
	void GiveTurnToPlayer();
	void GiveTurnToEnemy();

	void AddCharacterToArrays(class AGameplayCharacter* Character);
	void DropCharacterFromArrays(class AGameplayCharacter* Character);

	void EmptyDeathList();

};
