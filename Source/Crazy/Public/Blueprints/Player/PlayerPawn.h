// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Structs/TileDefinition.h"
#include "Blueprints/Grid/GridManagerActor.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"
#include "Blueprints/UI/HUDWidget.h"
#include "PlayerPawn.generated.h"

UCLASS()
class CRAZY_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

	UPROPERTY()
	UCameraComponent* MainCamera;
	UPROPERTY(editanywhere, Category = "Camera|Movement")
	float cameraXSpeed = 1.f;
	UPROPERTY(editanywhere, Category = "Camera|Movement")
	float cameraYSpeed = 1.f;
	UPROPERTY(editanywhere, Category = "Camera|Zoom")
	float cameraZoomSpeed = 1.f;
	UPROPERTY(editanywhere, Category = "Camera|Zoom")
	float cameraMaxZoom = 1.f;
	UPROPERTY(editanywhere, Category = "Camera|Zoom")
	float cameraMinZoom = 1.f;
	UPROPERTY(editanywhere, Category = "Camera|Zoom")
	float CameraLerpSpeed = 1.f;
	UPROPERTY()
	float targetCameraZoom;

	UPROPERTY()
	AGridManagerActor* Grid;
	UPROPERTY()
	int HoveredTile;

	UPROPERTY(EditAnywhere, Category = "UI|HoveredTile")
	TSubclassOf<AActor> HoveredTileWidgetClass;
	UPROPERTY()
	AActor* HoveredTileWidget;
	
	UPROPERTY(EditAnywhere, Category = "UI|HoveredTile")
	TEnumAsByte<ETraceTypeQuery> TileTraceChannel = ETraceTypeQuery::TraceTypeQuery1;

	UPROPERTY(VisibleAnywhere)
	AGameplayCharacter* SelectedCharacter;
	UPROPERTY(VisibleAnywhere)
	int SelectedSkillIndex = -1;

	//UI
	UPROPERTY()
	UHUDWidget* HUDInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ManageCamera(float DeltaTime);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

	void UpdateHoveredTile();

	void ManageInputCameraX(float input);
	void ManageInputCameraY(float input);
	void ManageInputCameraZoom(float input);
	void ManageInputInteraction1();
	void ManageInputInteraction2();

};
