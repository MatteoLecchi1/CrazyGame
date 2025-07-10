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
void AGridManagerActor::OnConstruction(const FTransform& Transform)
{
	SpawnGrid(sizeX, sizeY);
}
// Called when the game starts or when spawned
void AGridManagerActor::BeginPlay()
{
	Super::BeginPlay();

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

	FTransform Spawntransform;
	Spawntransform.SetScale3D(FVector(1.f, 1.f, 1.f));
	Spawntransform.SetRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, 0.f)));
	
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
			newTile.XID = X;
			newTile.YID = Y;
			newTile.tileType = TileType::WALKABLE;
			newTile.Location = Spawntransform.GetLocation();

			Tiles.Add(newTile);
		}
	}
}

FTileDefinition AGridManagerActor::GetTileAtLocation(FVector location)
{
	FVector localLocation = location - GetActorLocation();

	int X = FMath::RoundToInt(localLocation.X / TileSize);
	int Y = FMath::RoundToInt(localLocation.Y / TileSize);

	for (FTileDefinition tile : Tiles)
	{
		if (tile.XID == X && tile.YID == Y)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Yellow, FString::Printf(TEXT("%lld"), Y));
			DrawDebugSphere(GetWorld(), GetActorTransform().TransformPosition(tile.Location), 40, 10, FColor::Red, false, 0.1);
			return tile;
		}
	}
	FTileDefinition newTile;
	return newTile;
}