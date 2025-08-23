// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Blueprints/Grid/GridManagerActor.h"
#include "Blueprints/Core/GameplayGameMode.h"
#include "GameplayPawn.generated.h"

UCLASS()
class CRAZY_API AGameplayPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGameplayPawn();

	UPROPERTY()
	AGridManagerActor* Grid;
	UPROPERTY()
	AGameplayGameMode* GameMode;

	UPROPERTY()
	int CurrentAP = 0;
	UPROPERTY()
	int MaxAP = 0;
	UPROPERTY()
	int APregen = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Initialize();
	virtual void OnTurnStart();
	virtual void SetAP(int APAmmount);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
