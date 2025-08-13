// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Structs/TileDefinition.h"
#include "GridManagerActor.generated.h"

UCLASS()
class CRAZY_API AGridManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridManagerActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent;
	UPROPERTY(editanywhere, Category = "Grid")
	int sizeX = 10;
	UPROPERTY(editanywhere, Category = "Grid")
	int sizeY = 10;
	UPROPERTY()
	float TileSize = 100.f;
	UPROPERTY(editanywhere)
	TEnumAsByte<ECollisionChannel> FloorCollisionChannel = ECC_Pawn;
	UPROPERTY(editanywhere)
	TEnumAsByte<ECollisionChannel> ObstructionChannel = ECC_Pawn;

	UPROPERTY(VisibleAnywhere)
	TMap<FInt32Vector2,FTileDefinition> Tiles;
	UPROPERTY()
	class APathFindingActor* PathFindingActor;
	UPROPERTY()
	TArray<FInt32Vector2> CardinalDirections;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void SpawnGrid(int TilesX, int TilesY);
	void SetCardinalDirections();
	FInt32Vector2 GetTileAtLocation(FVector location);
	int CalculateDistance(FInt32Vector2 Tile1, FInt32Vector2 Tile2);
	FHitResult CheckForObstruction(FInt32Vector2 StartTile, FInt32Vector2 EndTile);
	FHitResult CheckForObstructionBetweenLocations(FVector traceStart, FVector traceEnd);
	FTileDefinition* GetTileDefinition(FInt32Vector2 TileKey);
	TArray<FInt32Vector2> FindPath(FInt32Vector2 StartTile, FInt32Vector2 EndTile); 
	TArray<FInt32Vector2> GetValidTileNeighbors(FInt32Vector2 StartTile);
	TArray<FPathFindingData> GetValidTileNeighborsPathFindingData(FInt32Vector2 StartTile);
};
