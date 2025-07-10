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

USTRUCT(BlueprintType)
struct CRAZY_API FTileDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int XID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int YID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TileType tileType;
};
