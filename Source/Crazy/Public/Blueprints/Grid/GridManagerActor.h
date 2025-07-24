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
	TArray<FTileDefinition> Tiles;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void SpawnGrid(int TilesX, int TilesY);
	int GetTileAtLocation(FVector location);
	int CalculateDistance(int Tile1, int Tile2);
	FHitResult CheckForObstruction(int StartTile, int EndTile);
};
