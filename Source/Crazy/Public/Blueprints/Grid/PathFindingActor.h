// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprints/Grid/GridManagerActor.h"
#include "PathFindingActor.generated.h"

UCLASS()
class CRAZY_API APathFindingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathFindingActor();
	UPROPERTY()
	AGridManagerActor* GridManager;

	UPROPERTY()
	FInt32Vector2 CurrentStartTile;
	UPROPERTY()
	FInt32Vector2 CurrentEndTile;
	UPROPERTY()
	TArray<FInt32Vector2> DiscoveredTilesIndexes;
	UPROPERTY()
	TArray<int> DiscoveredTilesSortingCost;
	UPROPERTY()
	TArray<FInt32Vector2> AnalysedTilesIndexes;
	UPROPERTY()
	TMap<FInt32Vector2, FPathFindingData> PathFindingData;
	UPROPERTY()
	FPathFindingData CurrentDiscoveredTile;

	UPROPERTY()
	TArray<FPathFindingData> CurrentNeighbors;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<FInt32Vector2> FindPath(FInt32Vector2 StartTile, FInt32Vector2 EndTile);
	void DiscoverTile(FPathFindingData TileData);
	bool AnalyseNextDiscoveredTile();
	int GetMinimumCostBetweenTwoTiles(FInt32Vector2 StartTile, FInt32Vector2 EndTile);
	TArray<FInt32Vector2> GeneratePath();
	FPathFindingData PullCheapestTileOutOfDiscoveredList();
	void InsertTileInDiscoveredArray(FPathFindingData TileData);
	bool DiscoverNextNeighbor();
	void CleanAllData();
};
