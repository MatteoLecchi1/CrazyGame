// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Player/PlayerPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
}
void APlayerPawn::Initialize()
{
	MainCamera = Cast<UCameraComponent>(GetComponentsByTag(UCameraComponent::StaticClass(), "MainCamera")[0]);
	targetCameraZoom = MainCamera->OrthoWidth;

	Grid = Cast<AGridManagerActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManagerActor::StaticClass()));

	if (HoveredTileWidgetClass)
		HoveredTileWidget = GetWorld()->SpawnActor<AActor>(HoveredTileWidgetClass, FVector(0.f, 0.f, 9999999.f), GetActorRotation());
	SetActorTickEnabled(true);
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

	int newTile = Grid->GetTileAtLocation(CursorHit.Location);
	
	if (newTile < 0)
		return;
	if (newTile == HoveredTile)
		return;

	HoveredTile = newTile;

	if(HoveredTileWidget)
	HoveredTileWidget->SetActorLocation(Grid->Tiles[HoveredTile].Location);
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("CameraX", this, &APlayerPawn::ManageInputCameraX);
	InputComponent->BindAxis("CameraY", this, &APlayerPawn::ManageInputCameraY);
	InputComponent->BindAxis("CameraZoom", this, &APlayerPawn::ManageInputCameraZoom);
	InputComponent->BindAction("Interact1", IE_Pressed, this, &APlayerPawn::ManageInputInteraction1);
	InputComponent->BindAction("Interact2", IE_Pressed, this, &APlayerPawn::ManageInputInteraction2);
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
void APlayerPawn::ManageInputInteraction1()
{
	if (Grid->Tiles[HoveredTile].Occupant)
	{
		AGameplayCharacter* targetedCharacter = Cast<AGameplayCharacter>(Grid->Tiles[HoveredTile].Occupant);
		if (SelectedSkillIndex < 0) 
		{
			if (targetedCharacter->Faction == Factions::PLAYER)
			{
				if (SelectedCharacter == targetedCharacter)
					return;

				SelectedSkillIndex = -1;
				SelectedCharacter = targetedCharacter;
				HUDInstance->UpdateSkillList(SelectedCharacter->Skills);
			}
		}
		else
		{
			if (!SelectedCharacter)
				return;
			if (Grid->CheckForObstruction(SelectedCharacter->CurrentTile, HoveredTile).bBlockingHit)
				return;

			SelectedCharacter->UseSkill(SelectedCharacter->Skills[SelectedSkillIndex], HoveredTile);
		}
	}
	else
	{
		if (!SelectedCharacter)
			return;
		if (SelectedSkillIndex >= 0)
			return;

		SelectedCharacter->WalkToTile(HoveredTile);
	}
}
void APlayerPawn::ManageInputInteraction2()
{
	if(SelectedSkillIndex >= 0)
	{
		SelectedSkillIndex = -1;
	}
	else
	{
		SelectedCharacter = nullptr;

		TArray<FSkillDefinition> EmptySkills;
		HUDInstance->UpdateSkillList(EmptySkills);
	}
}