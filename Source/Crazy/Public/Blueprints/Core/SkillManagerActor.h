// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/TileDefinition.h"
#include "Blueprints/Grid/GridManagerActor.h"
#include "Blueprints/Core/SkillEffect.h"
#include "Blueprints/Debuffs/Debuff.h"
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
	UPROPERTY(Instanced,EditAnywhere)
	TMap<FName,USkillEffect*> SkillEffects;
	UPROPERTY(EditAnywhere)
	TMap<FName, TSubclassOf<UDebuff>> Debuffs;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ManageSkill(FSkillDefinition* skillUsed, FIntVector2 targetedTile, FIntVector2 StartTile, AGameplayCharacter* SkillUser);
	float CheckManageSkill(FSkillDefinition* skillUsed, FIntVector2 targetedTile, FIntVector2 StartTile, AGameplayCharacter* SkillUser);
	TArray<FIntVector2> FindSkillAOE(FSkillDefinition* skillUsed, FIntVector2 targetedTile, FIntVector2 StartTile);
	TArray<FIntVector2> FindAOESkillAOE(FSkillDefinition* skillUsed, FIntVector2 targetedTile);
	TArray<FIntVector2> FindDIRECTIONALAOESkillAOE(FSkillDefinition* skillUsed, FIntVector2 targetedTile, FIntVector2 StartTile);

	TArray<AGameplayCharacter*> FindSkillTargets(FSkillDefinition* skillUsed, TArray<FIntVector2> targetedTiles, FIntVector2 targetedTile, AGameplayCharacter* SkillUser);
	float PlaySkill(FSkillDefinition* skillUsed, FIntVector2 targetedTile, TArray<AGameplayCharacter*> Targets, AGameplayCharacter* SkillUser);
	float ApplyDebuff(AGameplayCharacter* Target, AGameplayCharacter* InstigatingCharacter, FName DebuffKey);
	float CheckPlaySkill(FSkillDefinition* skillUsed, TArray<AGameplayCharacter*> Targets, AGameplayCharacter* SkillUser);
};
