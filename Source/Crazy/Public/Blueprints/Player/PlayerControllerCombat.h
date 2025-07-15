// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprints/Player/PlayerPawn.h"
#include "Blueprints/UI/HUDWidget.h"
#include "PlayerControllerCombat.generated.h"

/**
 * 
 */
UCLASS()
class CRAZY_API APlayerControllerCombat : public APlayerController
{
	GENERATED_BODY()

public:

	UPROPERTY()
	APlayerPawn* PlayerPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UHUDWidget> HUDClass;
	UPROPERTY()
	UHUDWidget* HUDInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
