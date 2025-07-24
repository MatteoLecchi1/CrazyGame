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

			FTileDefinition newTile;
			newTile.X = X;
			newTile.Y = Y;
			newTile.Location = GetActorTransform().TransformPosition(Spawntransform.GetLocation());

			Tiles.Add(newTile);
		}
	}
}

int AGridManagerActor::GetTileAtLocation(FVector location)
{
	FVector localLocation = GetActorTransform().InverseTransformPosition(location);

	int X = FMath::RoundToInt(localLocation.X / TileSize);
	int Y = FMath::RoundToInt(localLocation.Y / TileSize);

	for (int i = 0; i < Tiles.Num(); i++)
	{
		if (Tiles[i].X == X && Tiles[i].Y == Y)
		{
			//DrawDebugSphere(GetWorld(), tile.Location, 40, 10, FColor::Red, false, 0.1);
			return i;
		}
	}
	return -1;
}

int AGridManagerActor::CalculateDistance(int Tile1, int Tile2)
{
	return
		FMath::Abs(Tiles[Tile1].X - Tiles[Tile2].X) +
		FMath::Abs(Tiles[Tile1].Y - Tiles[Tile2].Y);
}

FHitResult AGridManagerActor::CheckForObstruction(int StartTile, int EndTile)
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;

	FVector traceStart = FVector(Tiles[StartTile].Location.X, Tiles[StartTile].Location.Y,5.f);
	FVector traceEnd = FVector(Tiles[EndTile].Location.X, Tiles[EndTile].Location.Y, 5.f);

	GetWorld()->LineTraceSingleByChannel(HitResult, traceStart, traceEnd, ObstructionChannel, CollisionParams);
	if (HitResult.bBlockingHit)
		DrawDebugLine(GetWorld(), traceStart, HitResult.Location, FColor::Green, false);
	else
		DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Red, false);
	return HitResult;
}