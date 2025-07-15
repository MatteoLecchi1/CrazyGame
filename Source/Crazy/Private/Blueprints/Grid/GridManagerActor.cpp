// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Grid/GridManagerActor.h"
#include "Chaos/DebugDrawQueue.h"

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
	{
		return;
	}

	InstancedStaticMeshComponent->ClearInstances();
	Tiles.Empty();

	FTransform Spawntransform = FTransform(
		FQuat::MakeFromEuler(FVector(0.f, 0.f, 0.f)),
		FVector(0.f, 0.f, 0.f),
		FVector(1.f, 1.f, 1.f));
	
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


			GetWorld()->LineTraceSingleByChannel(Hitfront, WorldTraceStart, WorldTraceEnd, TraceChannelProperty, CollisionParams);
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