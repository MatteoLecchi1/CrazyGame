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
};

UENUM(BlueprintType)
enum class Factions : uint8
{
	PLAYER = 0		UMETA(DisplayName = "PLAYER"),
	BANDITS = 1		UMETA(DisplayName = "BANDITS"),
	MONSTERS = 2	UMETA(DisplayName = "MONSTERS"),
};

USTRUCT(BlueprintType)
struct CRAZY_API FTileDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Y = 0;

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
	int DamageAmmount = 0;

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
	TSubclassOf<UTexture2D> SkillIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDamageDefinition> Damage;
};