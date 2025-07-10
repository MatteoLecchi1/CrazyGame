// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Player/PlayerPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	MainCamera = Cast<UCameraComponent>(GetComponentsByTag(UCameraComponent::StaticClass(), "MainCamera")[0]);
	targetCameraZoom = MainCamera->OrthoWidth;

	Grid = Cast<AGridManagerActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManagerActor::StaticClass()));
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ManageCamera(DeltaTime);
	UpdateHoveredTile();
}

void APlayerPawn::ManageCamera(float DeltaTime)
{
	MainCamera->OrthoWidth = FMath::Lerp(MainCamera->OrthoWidth, targetCameraZoom, DeltaTime * CameraLerpSpeed);
}

void APlayerPawn::UpdateHoveredTile()
{
	FHitResult CursorHit;
	UGameplayStatics::GetPlayerController(GetWorld(),0)->GetHitResultUnderCursorByChannel(TileTraceChannel,false, CursorHit);
	if (!CursorHit.bBlockingHit)
		return;
	HoveredTile = Grid->GetTileAtLocation(CursorHit.Location);
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("CameraX", this, &APlayerPawn::ManageInputCameraX);
	InputComponent->BindAxis("CameraY", this, &APlayerPawn::ManageInputCameraY);
	InputComponent->BindAxis("CameraZoom", this, &APlayerPawn::ManageInputCameraZoom);
}
void APlayerPawn::ManageInputCameraX(float input)
{
	AddActorWorldOffset(FVector(input * cameraXSpeed, 0, 0));
}

void APlayerPawn::ManageInputCameraY(float input)
{
	AddActorWorldOffset(FVector(0, input * cameraYSpeed, 0));
}

void APlayerPawn::ManageInputCameraZoom(float input)
{
	targetCameraZoom += input * cameraZoomSpeed;

	targetCameraZoom = FMath::Clamp(targetCameraZoom, cameraMinZoom, cameraMaxZoom);
}