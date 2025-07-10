// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
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
	float SquareSize = 100.f;
	UPROPERTY()
	TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Pawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void SpawnGrid(int TilesX, int TilesY);
};
