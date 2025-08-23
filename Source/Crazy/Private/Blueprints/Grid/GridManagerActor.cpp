// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Grid/GridManagerActor.h"
#include "Chaos/DebugDrawQueue.h"
#include "DrawDebugHelpers.h"
#include "Blueprints/Grid/PathFindingActor.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"
#include "Blueprints/Core/GameplayGameMode.h"

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
void AGridManagerActor::Initialize() 
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PathFindingActor = GetWorld()->SpawnActor<APathFindingActor>();
	PathFindingActor->GridManager = this;

	SetCardinalDirections();
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

	for (int X = 0; X < TilesX; X++)
	{
		for (int Y = 0; Y < TilesY; Y++)
		{
			SpawnSpawnSingleTile(X, Y);
		}
	}
}

void AGridManagerActor::SpawnSpawnSingleTile(int X, int Y)
{
	FTransform Spawntransform = FTransform::Identity;
	FHitResult Hitfront;

	float TileXLocation = X * TileSize;
	float TileYLocation = Y * TileSize;

	FVector TraceStart = FVector(TileXLocation, TileYLocation, 100.f);
	FVector TraceEnd = FVector(TileXLocation, TileYLocation, -100.f);

	FVector WorldTraceStart = GetActorTransform().TransformPosition(TraceStart);
	FVector WorldTraceEnd = GetActorTransform().TransformPosition(TraceEnd);


	GetWorld()->LineTraceSingleByChannel(Hitfront, WorldTraceStart, WorldTraceEnd, FloorCollisionChannel);
	if (!Hitfront.bBlockingHit)
		return;
	if (!Hitfront.GetActor()->ActorHasTag(FloorTag))
		return;

	Spawntransform.SetLocation(FVector(TileXLocation, TileYLocation, 0.1f));
	InstancedStaticMeshComponent->AddInstance(Spawntransform);

	FInt32Vector2 newKey;
	newKey.X = X;
	newKey.Y = Y;

	FTileDefinition newTile;
	newTile.Location = GetActorTransform().TransformPosition(Spawntransform.GetLocation());

	Tiles.Add(newKey, newTile);
}

void AGridManagerActor::SetCardinalDirections() 
{
	CardinalDirections.Empty();
	CardinalDirections.Add(FInt32Vector2(0, 1));
	CardinalDirections.Add(FInt32Vector2(0, -1));
	CardinalDirections.Add(FInt32Vector2(1, 0));
	CardinalDirections.Add(FInt32Vector2(-1, 0));
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

	return HitResult;
}
FHitResult AGridManagerActor::CheckForObstructionBetweenLocations(FVector traceStart, FVector traceEnd)
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;

	traceStart.Z = 5.f;
	traceEnd.Z = 5.f;
	GetWorld()->LineTraceSingleByChannel(HitResult, traceStart, traceEnd, ObstructionChannel, CollisionParams);

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

TArray<FInt32Vector2> AGridManagerActor::FindPath(FInt32Vector2 StartTile, FInt32Vector2 EndTile) 
{
	return PathFindingActor->FindPath(StartTile, EndTile);
}

AGameplayCharacter* AGridManagerActor::FindClosestCharacter(AGameplayCharacter* Startcharacter, TArray<AGameplayCharacter*> characters)
{

	auto targetsNeighbors = GetValidTileNeighbors(Startcharacter->CurrentTile);
	for (auto Neighbor : targetsNeighbors)
	{
		auto definition = GetTileDefinition(Neighbor);
		if (definition->Occupant)
		{
			auto target = Cast<AGameplayCharacter>(definition->Occupant);
			if(characters.Contains(target))
				return target;
		}
	}

	TArray<FInt32Vector2> Currentpath;
	TArray<FInt32Vector2> path;
	AGameplayCharacter* CurrentClosestCharacter = nullptr;

	auto GameMode = Cast<AGameplayGameMode>(GetWorld()->GetAuthGameMode());

	int i = 0;

	for (auto character : characters) 
	{
		TArray<FInt32Vector2> characterNeighbors = GetValidTileNeighbors(character->CurrentTile);

		if (characterNeighbors.Num() == 0)
			continue;

		Currentpath = FindPath(Startcharacter->CurrentTile, characterNeighbors[0]);

		if (Currentpath.Num() == 0)
			continue;
		
		if (Currentpath.Num() < path.Num() || i == 0) 
		{
			path = Currentpath;
			CurrentClosestCharacter = character;
			i++;
		}
	}
	return CurrentClosestCharacter;
}

TArray<FInt32Vector2> AGridManagerActor::GetValidTileNeighbors(FInt32Vector2 StartTile)
{
	TArray<FInt32Vector2> TilesFound;
	for (auto direction : CardinalDirections)
	{
		direction += StartTile;
		if (auto definition = GetTileDefinition(direction)) 
		{
			if (definition->Occupant)
				continue;
			if (definition->tileType != TileType::WALKABLE)
				continue;

			TilesFound.Add(direction);
		}
	}
	return TilesFound;
}
TArray<FPathFindingData> AGridManagerActor::GetValidTileNeighborsPathFindingData(FPathFindingData StartTile)
{
	TArray<FPathFindingData> TilesFound;
	for (auto direction : CardinalDirections)
	{
		direction += StartTile.Index;

		if (StartTile.PreviousIndex == direction)
			continue;

		if (auto definition = GetTileDefinition(direction))
		{
			if (definition->Occupant)
				continue;
			if (definition->tileType != TileType::WALKABLE)
				continue;

			FPathFindingData TileDataFound;
			TileDataFound.Index = direction;
			TileDataFound.PreviousIndex = StartTile.Index;
			TileDataFound.CostToEnterTile = 1;
			TilesFound.Add(TileDataFound);
		}
	}
	return TilesFound;
}