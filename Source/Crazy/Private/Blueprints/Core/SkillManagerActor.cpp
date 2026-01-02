// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Core/SkillManagerActor.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"
#include "Chaos/DebugDrawQueue.h"

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

void ASkillManagerActor::ManageSkill(FSkillDefinition* skillUsed, FIntVector2 targetedTile, FIntVector2 StartTile, AGameplayCharacter* SkillUser)
{
	TArray<FIntVector2> AOETiles = FindSkillAOE(skillUsed, targetedTile, StartTile);
	TArray<AGameplayCharacter*> Targets = FindSkillTargets(skillUsed, AOETiles, targetedTile, SkillUser);
	PlaySkill(skillUsed, targetedTile, Targets, SkillUser);
}

float ASkillManagerActor::CheckManageSkill(FSkillDefinition* skillUsed, FIntVector2 targetedTile, FIntVector2 StartTile, AGameplayCharacter* SkillUser)
{
	TArray<FIntVector2> AOETiles = FindSkillAOE(skillUsed, targetedTile, StartTile);
	TArray<AGameplayCharacter*> Targets = FindSkillTargets(skillUsed, AOETiles, targetedTile, SkillUser);
	return CheckPlaySkill(skillUsed, Targets, SkillUser);
}

TArray<FIntVector2> ASkillManagerActor::FindSkillAOE(FSkillDefinition* skillUsed, FIntVector2 targetedTile, FIntVector2 StartTile)
{
	TArray<FIntVector2> AOETiles;

	switch (skillUsed->AOEtype)
	{
	case AOEType::SINGLETILE:
		AOETiles.Add(targetedTile);
		break;
	case AOEType::AOE:
		AOETiles = FindAOESkillAOE(skillUsed, targetedTile);
		break;
	case AOEType::DIRECTIONALAOE:
		AOETiles = FindDIRECTIONALAOESkillAOE(skillUsed, targetedTile, StartTile);
		targetedTile = StartTile;
		break;
	default:
		break;
	}
	for (int i = 0; i< AOETiles.Num(); i++)
	{
		if (Grid->GetTileDefinition(AOETiles[i]))
		{
			if(Grid->CheckForObstruction(targetedTile, AOETiles[i]).bBlockingHit)
			{
				AOETiles.RemoveAt(i);
				i--;
				continue;
			}
		}
		else
		{
			AOETiles.RemoveAt(i);
			i--;
		}
	}
	return AOETiles;
}
TArray<FIntVector2> ASkillManagerActor::FindAOESkillAOE(FSkillDefinition* skillUsed, FIntVector2 targetedTile)
{
	TArray<FIntVector2> AOETiles = skillUsed->AOEData.AOETiles;
	for (int i = 0; i <AOETiles.Num(); i++) 
	{
		AOETiles[i] += targetedTile;
	}
	return AOETiles;
}

TArray<FIntVector2> ASkillManagerActor::FindDIRECTIONALAOESkillAOE(FSkillDefinition* skillUsed, FIntVector2 targetedTile, FIntVector2 StartTile)
{
	FIntVector2 relativeTargetedTile = StartTile - targetedTile;
	int TargetAreaDirection = 0;
	if (abs(relativeTargetedTile.X) > abs(relativeTargetedTile.Y))
	{
		if (relativeTargetedTile.X > 0)
			TargetAreaDirection = 2;
		else
			TargetAreaDirection = 0;
	}
	else
	{
		if (relativeTargetedTile.Y > 0)
			TargetAreaDirection = 1;
		else
			TargetAreaDirection = 3;
	}

	TArray<FIntVector2> AOETiles = skillUsed->AOEData.AOETiles;
	
	for (int i = 0; i < AOETiles.Num(); i++)
	{
		AOETiles[i] = Grid->RotateOffset(AOETiles[i], TargetAreaDirection);
		AOETiles[i] += StartTile;
	}
	return AOETiles;
}

TArray<AGameplayCharacter*> ASkillManagerActor::FindSkillTargets(FSkillDefinition* skillUsed, TArray<FIntVector2> targetedTiles, FIntVector2 targetedTile, AGameplayCharacter* SkillUser)
{
	TArray<AGameplayCharacter*> Targets;
	for (auto tile : targetedTiles)
	{
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

float ASkillManagerActor::PlaySkill(FSkillDefinition* skillUsed, FIntVector2 targetedTile, TArray<AGameplayCharacter*> Targets, AGameplayCharacter* SkillUser)
{
	float reward = 0;
	for (auto effectKey : skillUsed->SkillEffects)
	{
		if (USkillEffect** effect = SkillEffects.Find(effectKey))
		{
			reward += (*effect)->PlaySkillEffect(SkillUser, targetedTile, *skillUsed, Targets, Grid);
		}
	}
	for (auto DebuffKey : skillUsed->Debuffs)
	{
		for (auto Target : Targets)
		{
			if (TSubclassOf<UDebuff>* Debuff = Debuffs.Find(DebuffKey))
			{
				TSubclassOf<UDebuff> DebuffClass = (*Debuff);
				UDebuff* CurrentDebuff = NewObject<UDebuff>(Target,DebuffClass);
				Target->Debuffs.Add(CurrentDebuff);

				CurrentDebuff->DebuffedCharacter = Target;
				reward += CurrentDebuff->OnDebuffApplyed(Grid, SkillUser);

				FString TheFloatStr = CurrentDebuff->GetClass()->GetName();
				GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
			}
		}
	}
	GameMode->EmptyDeathList();
	return reward;
}

float ASkillManagerActor::CheckPlaySkill(FSkillDefinition* skillUsed, TArray<AGameplayCharacter*> Targets, AGameplayCharacter* SkillUser)
{
	float reward = 0;
	for (AGameplayCharacter* Target : Targets)
	{
		if (Target == SkillUser)
			continue;
		for (auto damageInstance : skillUsed->Damage)
		{
			float damage = Target->CheckInflictedDamage(damageInstance.DamageAmount, damageInstance.DamageElement);
			float rewardWeight = 1;

			if (Target->Faction == SkillUser->Faction)
				rewardWeight = SkillUser->FriendlyDamageWeight;
			else
				rewardWeight = SkillUser->DamageWeight;

			reward += damage * rewardWeight;
		}
	}
	return reward;
}



