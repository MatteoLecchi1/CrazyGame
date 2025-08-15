// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/Core/GameplayPawn.h"
#include "Camera/CameraComponent.h"
#include "Structs/TileDefinition.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"
#include "Blueprints/UI/HUDWidget.h"
#include "PlayerPawn.generated.h"

UCLASS()
class CRAZY_API APlayerPawn : public AGameplayPawn
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
	FInt32Vector2 HoveredTile;
	UPROPERTY(EditAnywhere, Category = "UI|HoveredTile")
	TEnumAsByte<ETraceTypeQuery> TileTraceChannel = ETraceTypeQuery::TraceTypeQuery1;

	//UI
	UPROPERTY(EditAnywhere, Category = "UI|HoveredTile")
	TSubclassOf<AActor> HoveredTileWidgetClass;
	UPROPERTY()
	AActor* HoveredTileWidget;
	UPROPERTY()
	TArray<AActor*> WalkingTileWidgets;
	UPROPERTY(EditAnywhere, Category = "UI|TileVisuals")
	TSubclassOf<AActor> WalkingTileWidgetclass;
	UPROPERTY()
	AActor* SkillWidget;
	UPROPERTY(EditAnywhere, Category = "UI|TileVisuals")
	TSubclassOf<AActor> SkillWidgetclass;
	UPROPERTY(EditAnywhere, Category = "UI|TileVisuals")
	TSubclassOf<AActor> InvalidSkillWidgetclass;
	UPROPERTY()
	UHUDWidget* HUDInstance;
	UPROPERTY()
	PlayerSelectionState SelectionState = PlayerSelectionState::NONE;
	

	UPROPERTY(VisibleAnywhere)
	AGameplayCharacter* SelectedCharacter;
	UPROPERTY(VisibleAnywhere)
	int SelectedSkillIndex = -1;

	UPROPERTY()
	bool PlayersTurn = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Initialize() override;

	void ManageCamera(float DeltaTime);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

	void UpdateHoveredTile();
	void UpdateStateVisuals();
	void UpdateFRIENDLYCHARACTERStateVisuals();
	void UpdateSKILLStateVisuals();
	void DestroyFRIENDLYCHARACTERStateVisuals();
	void DestroySKILLStateVisuals();

	void EndTurn();
	void SetAP(int APAmmount) override;

	void ManageInputCameraX(float input);
	void ManageInputCameraY(float input);
	void ManageInputCameraZoom(float input);
	void ManageInputInteraction1();
	void ManageInputInteraction2();
	void ManageInputEndTurn();

	void Interaction1NONE();
	void Interaction1FRIENDLYCHARACTER(); 
	void Interaction1SKILL();

	void Interaction2NONE();
	void Interaction2FRIENDLYCHARACTER();
	void Interaction2SKILL();
};
