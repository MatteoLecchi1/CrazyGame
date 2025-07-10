// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Grid/GridManagerActor.h"

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
	float gridXOffset = TilesX * SquareSize / 2;
	float gridYOffset = TilesY * SquareSize / 2;

	if (TilesX % 2 != 0)
		gridXOffset -= SquareSize / 2;
	if (TilesY % 2 != 0)
		gridYOffset -= SquareSize / 2;

	FTransform Spawntransform;
	Spawntransform.SetScale3D(FVector(1.f, 1.f, 1.f));
	Spawntransform.SetRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, 0.f)));
	
	FCollisionQueryParams CollisionParams;
	FHitResult Hitfront;

	for (int X = 0; X < TilesX; X++)
	{
		for (int Y = 0; Y < TilesY; Y++)
		{
			float TileXLocation = X * SquareSize - gridXOffset;
			float TileYLocation = Y * SquareSize - gridYOffset;

			FVector TraceStart = FVector(TileXLocation, TileYLocation, 100.f);
			FVector TraceEnd = FVector(TileXLocation, TileYLocation, -100.f);

			GetWorld()->LineTraceSingleByChannel(Hitfront, TraceStart, TraceEnd, TraceChannelProperty, CollisionParams);
			if (!Hitfront.bBlockingHit)
				continue;

			Spawntransform.SetLocation(FVector(TileXLocation, TileYLocation, 0.1f));
			InstancedStaticMeshComponent->AddInstance(Spawntransform);
		}
	}
}
