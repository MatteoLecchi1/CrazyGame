// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Grid/PathFindingActor.h"
#include "Chaos/DebugDrawQueue.h"

// Sets default values
APathFindingActor::APathFindingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APathFindingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APathFindingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
TArray<FIntVector2> APathFindingActor::FindPath(FIntVector2 StartTile, FIntVector2 EndTile)
{
	CleanAllData();
	TArray<FIntVector2> EmptyPath;
	CurrentStartTile = StartTile;
	CurrentEndTile = EndTile;

	if (StartTile == EndTile)
		return EmptyPath;

	FPathFindingData StartTileData;
	StartTileData.Index = StartTile;
	StartTileData.CostFromStart = 0;
	StartTileData.MinimumCostToEnd = GetMinimumCostBetweenTwoTiles(StartTile, EndTile);
	DiscoverTile(StartTileData);

	while (DiscoveredTilesIndexes.Num() > 0)
	{
		bool foundTargetTile = AnalyseNextDiscoveredTile();
		if (foundTargetTile)
		{
			return GeneratePath();
		}
	}
	//path not found
	return EmptyPath;
}
void APathFindingActor::DiscoverTile(FPathFindingData TileData) 
{
	PathFindingData.Add(TileData.Index,TileData);
	InsertTileInDiscoveredArray(TileData);
}
bool APathFindingActor::AnalyseNextDiscoveredTile()
{
	CurrentDiscoveredTile = PullCheapestTileOutOfDiscoveredList();
	CurrentNeighbors = GridManager->GetValidTileNeighborsPathFindingData(CurrentDiscoveredTile,CurrentEndTile);

	while (CurrentNeighbors.Num() > 0)
	{
		bool foundTargetTile = DiscoverNextNeighbor();
		if (foundTargetTile)
			return foundTargetTile;
	}
	return false;
}
int APathFindingActor::GetMinimumCostBetweenTwoTiles(FIntVector2 StartTile, FIntVector2 EndTile) {
	return GridManager->CalculateDistance(StartTile, EndTile);
}
TArray<FIntVector2> APathFindingActor::GeneratePath()
{
	FIntVector2 current = CurrentEndTile;
	TArray<FIntVector2> InvertedPath;
	TArray<FIntVector2> ReturnPath;

	if(GridManager->GetTileDefinition(current)->Occupant)
	{
		FPathFindingData* Next = PathFindingData.Find(current);
		current = Next->PreviousIndex;
	}

	while (current != CurrentStartTile) 
	{
		InvertedPath.Add(current);
		FPathFindingData* Next = PathFindingData.Find(current);
		current = Next->PreviousIndex;
	}
	for (int i = InvertedPath.Num()-1; i >= 0; i--) 
	{
		ReturnPath.Add(InvertedPath[i]);
	}
	return ReturnPath;
}
FPathFindingData APathFindingActor::PullCheapestTileOutOfDiscoveredList()
{
	FIntVector2 tileIndex =  DiscoveredTilesIndexes[0];
	FPathFindingData* currentDataIndex = PathFindingData.Find(tileIndex);
	FPathFindingData& currentData = *currentDataIndex;

	DiscoveredTilesSortingCost.RemoveAt(0);
	DiscoveredTilesIndexes.RemoveAt(0);

	AnalysedTilesIndexes.Add(tileIndex);

	return currentData;
}
void APathFindingActor::InsertTileInDiscoveredArray(FPathFindingData TileData)
{
	int sortingCost = TileData.CostFromStart + TileData.MinimumCostToEnd;
	if (DiscoveredTilesSortingCost.Num() == 0) 
	{
		DiscoveredTilesSortingCost.Add(sortingCost);
		DiscoveredTilesIndexes.Add(TileData.Index);
	}
	else if(sortingCost >= DiscoveredTilesSortingCost[DiscoveredTilesSortingCost.Num()-1])
	{
		DiscoveredTilesSortingCost.Add(sortingCost);
		DiscoveredTilesIndexes.Add(TileData.Index);
	}
	else
	{
		int i = 0;
		for (auto NextCost : DiscoveredTilesSortingCost) 
		{
			if (NextCost >= sortingCost) {
				DiscoveredTilesSortingCost.Insert(NextCost, i);
				DiscoveredTilesIndexes.Insert(TileData.Index, i);
				break;
			}
			i++;
		}
	}
}
bool APathFindingActor::DiscoverNextNeighbor()
{
	FPathFindingData CurrentNeighbor = CurrentNeighbors[0];
	CurrentNeighbors.RemoveAt(0);

	if (AnalysedTilesIndexes.Contains(CurrentNeighbor.Index))
		return false;

	int CostFromStart = CurrentNeighbor.CostToEnterTile + CurrentDiscoveredTile.CostFromStart;

	int indexInDiscovered = DiscoveredTilesIndexes.Find(CurrentNeighbor.Index);

	if (indexInDiscovered != -1) 
	{
		FPathFindingData* currentDataIndex = PathFindingData.Find(CurrentNeighbor.Index);
		FPathFindingData& currentData = *currentDataIndex;
		CurrentNeighbor = currentData;
		if (CostFromStart < CurrentNeighbor.CostFromStart) 
		{
			DiscoveredTilesSortingCost.RemoveAt(indexInDiscovered);
			DiscoveredTilesIndexes.RemoveAt(indexInDiscovered);
		}
		else 
		{
			return false;
		}
	}
	FPathFindingData NewData;
	NewData.Index = CurrentNeighbor.Index;
	NewData.CostToEnterTile = CurrentNeighbor.CostToEnterTile;
	NewData.CostFromStart = CostFromStart;
	NewData.MinimumCostToEnd = GetMinimumCostBetweenTwoTiles(CurrentNeighbor.Index,CurrentEndTile);
	NewData.PreviousIndex = CurrentDiscoveredTile.Index;

	DiscoverTile(NewData);

	if (CurrentNeighbor.Index == CurrentEndTile)
		return true;
	else
		return false;
}

void APathFindingActor::CleanAllData() 
{
	PathFindingData.Empty();
	DiscoveredTilesSortingCost.Empty();
	DiscoveredTilesIndexes.Empty();
	AnalysedTilesIndexes.Empty();
}

