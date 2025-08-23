// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileDefinition.generated.h"

/**
 * 
 */
class CRAZY_API TileDefinition
{
public:
	TileDefinition();
	~TileDefinition();
}; 

UENUM(BlueprintType)
enum class TileType : uint8
{
	WALKABLE = 0	UMETA(DisplayName = "WALKABLE"),
	NONWALKABLE = 1	UMETA(DisplayName = "NONWALKABLE"),
};

UENUM(BlueprintType)
enum class DamageElements : uint8
{
	SLASH = 0	UMETA(DisplayName = "SLASH"),
	PIERCE = 1	UMETA(DisplayName = "PIERCE"),
	BLUNT = 2	UMETA(DisplayName = "BLUNT"),
	HEAL = 3	UMETA(DisplayName = "HEAL"),
	MAGIC = 4	UMETA(DisplayName = "MAGIC"),
};

UENUM(BlueprintType)
enum class Factions : uint8
{
	PLAYER = 0		UMETA(DisplayName = "PLAYER"),
	BANDITS = 1		UMETA(DisplayName = "BANDITS"),
	MONSTERS = 2	UMETA(DisplayName = "MONSTERS"),
};
UENUM(BlueprintType)
enum class PlayerSelectionState : uint8
{
	NONE = 0		UMETA(DisplayName = "NONE"),
	FRIENDLYCHARACTER = 1		UMETA(DisplayName = "FRIENDLYCHARACTER"),
	SKILL = 2	UMETA(DisplayName = "SKILL"),
};

USTRUCT(BlueprintType)
struct CRAZY_API FTileDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Occupant = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TileType tileType = TileType::WALKABLE;
};

USTRUCT(BlueprintType)
struct CRAZY_API FDamageDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DamageAmount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	DamageElements DamageElement = DamageElements::SLASH;
};

USTRUCT(BlueprintType)
struct CRAZY_API FSkillDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SkillDysplayName = "Skill";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* SkillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDamageDefinition> Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MinRange = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxRange = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int APCost = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Cooldown = 1;
	UPROPERTY()
	int CurrentCooldown = 0;
};
USTRUCT(BlueprintType)
struct CRAZY_API FPathFindingData
{
	GENERATED_BODY()

	UPROPERTY()
	FInt32Vector2 Index = FInt32Vector2(-1,-1);
	UPROPERTY()
	int CostToEnterTile = 1;
	UPROPERTY()
	int CostFromStart = 999999;
	UPROPERTY()
	int MinimumCostToEnd = 999999;
	UPROPERTY()
	FInt32Vector2 PreviousIndex = FInt32Vector2(-1, -1);
};