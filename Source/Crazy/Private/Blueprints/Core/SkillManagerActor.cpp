// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Core/SkillManagerActor.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"

// Sets default values
ASkillManagerActor::ASkillManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASkillManagerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkillManagerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkillManagerActor::ManageSkill(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile)
{
	TArray<FInt32Vector2> AOETiles = FindSkillAOE(skillUsed, targetedTile);
	TArray<AGameplayCharacter*> Targets = FindSkillTargets(skillUsed, AOETiles, targetedTile);
	PlaySkill(skillUsed, Targets);
}

TArray<FInt32Vector2> ASkillManagerActor::FindSkillAOE(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile)
{
	TArray<FInt32Vector2> AOETiles;

	if (!skillUsed->IsAOE)
	{
		AOETiles.Add(FInt32Vector2(0,0));
	}
	else
	{
		AOETiles = skillUsed->AOETiles;
	}

	return AOETiles;
}

TArray<AGameplayCharacter*> ASkillManagerActor::FindSkillTargets(FSkillDefinition* skillUsed, TArray<FInt32Vector2> targetedTiles, FInt32Vector2 targetedTile)
{
	TArray<AGameplayCharacter*> Targets;
	for (auto tile : targetedTiles)
	{
		tile += targetedTile;
		auto tileDefinition = Grid->GetTileDefinition(tile);
		if (!tileDefinition)
			continue;
		if (!tileDefinition->Occupant)
			continue;
		if (AGameplayCharacter* target = Cast<AGameplayCharacter>(tileDefinition->Occupant))
		{
			Targets.Add(target);
		}
	}
	return Targets;
}

void ASkillManagerActor::PlaySkill(FSkillDefinition* skillUsed, TArray<AGameplayCharacter*> Targets)
{
	for (AGameplayCharacter* Target : Targets)
	{
		for (auto damageInstance : skillUsed->Damage)
		{
			Target->MyTakeDamage(damageInstance.DamageAmount, damageInstance.DamageElement);
		}
	}
}



