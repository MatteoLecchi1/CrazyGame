// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/Core/GameplayPawn.h"
#include "EnemyPawn.generated.h"

/**
 * 
 */
UCLASS()
class CRAZY_API AEnemyPawn : public AGameplayPawn
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:

	UPROPERTY(EditAnywhere)
	TArray<class AGameplayCharacter*> EnemyTurnOrder;

	UPROPERTY(EditAnywhere)
	float DelayBetweenTurns;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Initialize() override;

	UFUNCTION()
	void PlayCharacterTurn(class AGameplayCharacter* character);

	void OnTurnStart() override;
	UFUNCTION()
	void EndTurn();
};
