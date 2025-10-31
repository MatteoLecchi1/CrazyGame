// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/TileDefinition.h"
#include "Blueprints/Grid/GridManagerActor.h"
#include "SkillManagerActor.generated.h"

UCLASS()
class CRAZY_API ASkillManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkillManagerActor();


	UPROPERTY()
	class AGameplayGameMode* GameMode;
	UPROPERTY()
	AGridManagerActor* Grid;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ManageSkill(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, FInt32Vector2 StartTile, AGameplayCharacter* SkillUser);
	float CheckManageSkill(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, FInt32Vector2 StartTile, AGameplayCharacter* SkillUser);
	TArray<FInt32Vector2> FindSkillAOE(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, FInt32Vector2 StartTile);
	TArray<FInt32Vector2> FindAOESkillAOE(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile);
	TArray<FInt32Vector2> FindDIRECTIONALAOESkillAOE(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, FInt32Vector2 StartTile);

	TArray<AGameplayCharacter*> FindSkillTargets(FSkillDefinition* skillUsed, TArray<FInt32Vector2> targetedTiles, FInt32Vector2 targetedTile, AGameplayCharacter* SkillUser);
	void PlaySkill(FSkillDefinition* skillUsed, TArray<AGameplayCharacter*> Targets);
	float CheckPlaySkill(FSkillDefinition* skillUsed, TArray<AGameplayCharacter*> Targets, AGameplayCharacter* SkillUser);
};
