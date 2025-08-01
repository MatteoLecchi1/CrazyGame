// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Grid/GridManagerActor.h"
#include "Chaos/DebugDrawQueue.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGridManagerActor::AGridManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMeshComponent"));
	InstancedStaticMeshComponent->SetupAttachment(RootComponent);
	InstancedStaticMeshComponent->bDisableCollision = true;
}
// Called when the game starts or when spawned
void AGridManagerActor::BeginPlay()
{
	Super::BeginPlay();
}
void AGridManagerActor::OnConstruction(const FTransform& Transform)
{
	SpawnGrid(sizeX, sizeY);
}

// Called every frame
void AGridManagerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGridManagerActor::SpawnGrid(int TilesX, int TilesY)
{
	if (!InstancedStaticMeshComponent)
		return;

	InstancedStaticMeshComponent->ClearInstances();
	Tiles.Empty();

	FTransform Spawntransform = FTransform::Identity;
	
	FCollisionQueryParams CollisionParams;
	FHitResult Hitfront;

	for (int X = 0; X < TilesX; X++)
	{
		for (int Y = 0; Y < TilesY; Y++)
		{
			float TileXLocation = X * TileSize;
			float TileYLocation = Y * TileSize;

			FVector TraceStart = FVector(TileXLocation, TileYLocation, 100.f);
			FVector TraceEnd = FVector(TileXLocation, TileYLocation, -100.f);

			FVector WorldTraceStart = GetActorTransform().TransformPosition(TraceStart);
			FVector WorldTraceEnd = GetActorTransform().TransformPosition(TraceEnd);


			GetWorld()->LineTraceSingleByChannel(Hitfront, WorldTraceStart, WorldTraceEnd, FloorCollisionChannel, CollisionParams);
			if (!Hitfront.bBlockingHit)
				continue;

			Spawntransform.SetLocation(FVector(TileXLocation, TileYLocation, 0.1f));
			InstancedStaticMeshComponent->AddInstance(Spawntransform);

			FInt32Vector2 newKey;
			newKey.X = X;
			newKey.Y = Y;

			FTileDefinition newTile;
			newTile.Location = GetActorTransform().TransformPosition(Spawntransform.GetLocation());

			Tiles.Add(newKey,newTile);
		}
	}
}

FInt32Vector2 AGridManagerActor::GetTileAtLocation(FVector location)
{
	FVector localLocation = GetActorTransform().InverseTransformPosition(location);

	FInt32Vector2 TileKey;
	TileKey.X = FMath::RoundToInt(localLocation.X / TileSize);
	TileKey.Y = FMath::RoundToInt(localLocation.Y / TileSize);

	if (GetTileDefinition(TileKey))
		return TileKey;

	TileKey.X = -1;
	TileKey.Y = -1;

	return TileKey;
}

int AGridManagerActor::CalculateDistance(FInt32Vector2 Tile1, FInt32Vector2 Tile2)
{
	return
		FMath::Abs(Tile1.X - Tile2.X) +
		FMath::Abs(Tile1.Y - Tile2.Y);
}

FHitResult AGridManagerActor::CheckForObstruction(FInt32Vector2 StartTile, FInt32Vector2 EndTile)
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;

	FTileDefinition* startTileDefinition = GetTileDefinition(StartTile);
	FTileDefinition* endTileDefinition = GetTileDefinition(EndTile);

	FVector traceStart = FVector(startTileDefinition->Location.X, startTileDefinition->Location.Y,5.f);
	FVector traceEnd = FVector(endTileDefinition->Location.X, endTileDefinition->Location.Y, 5.f);

	GetWorld()->LineTraceSingleByChannel(HitResult, traceStart, traceEnd, ObstructionChannel, CollisionParams);
	if (HitResult.bBlockingHit)
		DrawDebugLine(GetWorld(), traceStart, HitResult.Location, FColor::Red, false,1.f);
	else
		DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Green, false,1.f);
	return HitResult;
}

FTileDefinition* AGridManagerActor::GetTileDefinition(FInt32Vector2 TileKey)
{
	if (Tiles.Contains(TileKey))
	{
		return &(Tiles[TileKey]);
	}

	return nullptr;
}