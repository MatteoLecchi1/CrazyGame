// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ManageCamera(float DeltaTime);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

	void ManageInputCameraX(float input);
	void ManageInputCameraY(float input);
	void ManageInputCameraZoom(float input);

};
