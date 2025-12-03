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
	FName FloorTag = "Floor";
	UPROPERTY(editanywhere)
	TEnumAsByte<ECollisionChannel> ObstructionChannel = ECC_Pawn;

	UPROPERTY(editanywhere)
	TEnumAsByte<ECollisionChannel> PushChannel = ECC_Pawn;

	UPROPERTY()
	TMap<FIntVector2,FTileDefinition> Tiles;
	UPROPERTY()
	class APathFindingActor* PathFindingActor;
	UPROPERTY()
	TArray<FIntVector2> CardinalDirections;

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Initialize();

	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void SpawnGrid(int TilesX, int TilesY);
	void SpawnSpawnSingleTile(int X, int Y);
	void SetCardinalDirections();
	FIntVector2 GetTileAtLocation(FVector location);

	int CalculateDistance(FIntVector2 Tile1, FIntVector2 Tile2);
	FHitResult CheckForObstruction(FIntVector2 StartTile, FIntVector2 EndTile);
	FHitResult CheckForObstructionUsingSphere(FIntVector2 StartTile, FIntVector2 EndTile, AActor* character);
	FHitResult CheckForObstructionBetweenLocations(FVector traceStart, FVector traceEnd);

	FTileDefinition* GetTileDefinition(FIntVector2 TileKey);
	TArray<FIntVector2> FindPath(FIntVector2 StartTile, FIntVector2 EndTile);
	class AGameplayCharacter* FindClosestCharacter(class AGameplayCharacter* Startcharacter, TArray<class AGameplayCharacter*> characters);
	TArray<FIntVector2> GetValidTileNeighbors(FIntVector2 StartTile);
	TArray<FPathFindingData> GetValidTileNeighborsPathFindingData(FPathFindingData CenterTile, FIntVector2 EndTile);
	FIntVector2 RotateOffset(FIntVector2 offset, int rotation);
};
