// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Player/PlayerPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Chaos/DebugDrawQueue.h"
#include "Blueprints/Core/SkillManagerActor.h"

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
	InputComponent->SetActive(false);
}
void APlayerPawn::Initialize()
{
	Super::Initialize();

	MainCamera = Cast<UCameraComponent>(GetComponentsByTag(UCameraComponent::StaticClass(), "MainCamera")[0]);
	targetCameraZoom = MainCamera->OrthoWidth;

	if (HoveredTileWidgetClass)
		HoveredTileWidget = GetWorld()->SpawnActor<AActor>(HoveredTileWidgetClass, FVector(0.f, 0.f, 9999999.f), GetActorRotation());
	GameMode->PlayerPawn = this;
	myEnableTick = true;
	InputComponent->SetActive(true);
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!myEnableTick)
		return;

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

	FInt32Vector2 newTile = Grid->GetTileAtLocation(CursorHit.Location);
	
	if (newTile.X < 0)
		return;
	if (newTile == HoveredTile)
		return;

	HoveredTile = newTile;

	if(HoveredTileWidget)
	{
		FTileDefinition* hoveredTileDefinition = Grid->GetTileDefinition(HoveredTile);
		HoveredTileWidget->SetActorLocation(hoveredTileDefinition->Location);
	}
	UpdateStateVisuals();
}

void APlayerPawn::UpdateStateVisuals()
{
	switch (SelectionState)
	{
	case PlayerSelectionState::NONE:
		break;
	case PlayerSelectionState::FRIENDLYCHARACTER:
		UpdateFRIENDLYCHARACTERStateVisuals();
		break;
	case PlayerSelectionState::SKILL:
		UpdateSKILLStateVisuals();
		break;
	default:
		break;
	}
}

void APlayerPawn::UpdateFRIENDLYCHARACTERStateVisuals()
{
	DestroyFRIENDLYCHARACTERStateVisuals();

	TArray<FInt32Vector2> path = Grid->FindPath(SelectedCharacter->CurrentTile, HoveredTile);
	int maxWalkDistance = SelectedCharacter->CurrentMovement + SelectedCharacter->MovementSpeed * CurrentAP;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	int i = 0;

	for (auto tile : path) 
	{
		if (maxWalkDistance <= 0)
			return;
		FVector tileLocation = Grid->GetTileDefinition(tile)->Location;

		if (i < WalkingTileWidgets.Num())
		{
			WalkingTileWidgets[i]->SetActorLocation(tileLocation);
			WalkingTileWidgets[i]->SetActorHiddenInGame(false);
		}
		else
		{
			AActor* widget = GetWorld()->SpawnActor<AActor>(WalkingTileWidgetclass, tileLocation, FRotator::ZeroRotator, SpawnInfo);
			WalkingTileWidgets.Add(widget);
		}
		i++;
		maxWalkDistance--;
	}
}
void APlayerPawn::UpdateSKILLStateVisuals() 
{
	DestroySKILLStateVisuals();

	FSkillDefinition currentSkill = SelectedCharacter->Skills[SelectedSkillIndex];
	int maxRange = currentSkill.MaxRange;
	int minRange = currentSkill.MinRange;
	int Distance = Grid->CalculateDistance(SelectedCharacter->CurrentTile, HoveredTile);

	float lenght = 1.f; 
	float Rotation = 0.f;

	FHitResult HitResult;
	bool isSkillValid = true;

	FVector traceStart = Grid->GetTileDefinition(SelectedCharacter->CurrentTile)->Location;
	FVector traceEnd = Grid->GetTileDefinition(HoveredTile)->Location;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (Distance <= minRange - 0.1f)
	{
		isSkillValid = false;
	}
	else if (Distance >= maxRange + 0.1f)
	{
		isSkillValid = false;
	}

	HitResult = Grid->CheckForObstructionBetweenLocations(traceStart, traceEnd);

	if (HitResult.bBlockingHit)
	{
		isSkillValid = false;
		traceEnd = HitResult.Location;
	}

	auto traceEndDirection = traceEnd;
	traceEndDirection -= traceStart;

	lenght = traceEndDirection.Length() / 100;

	traceEndDirection.Normalize();

	Rotation = FMath::RadiansToDegrees(FMath::Atan2(traceEndDirection.X, -traceEndDirection.Y));

	if (isSkillValid)
	{
		UpdateAOESKILLStateVisuals();
		if(SelectedCharacter->Skills[SelectedSkillIndex].AOEtype != AOEType::DIRECTIONALAOE)
		{
			SkillWidget = GetWorld()->SpawnActor<AActor>(SkillWidgetclass, traceEnd, FRotator::MakeFromEuler(FVector(0.f, 0.f, Rotation)), SpawnInfo);
			SkillWidget->SetActorScale3D(FVector(1.f, lenght, 1.f));
		}
	}
	else
	{
		if (SelectedCharacter->Skills[SelectedSkillIndex].AOEtype == AOEType::DIRECTIONALAOE)
		{
			UpdateAOESKILLStateVisuals();
		}
	}
}
void APlayerPawn::UpdateAOESKILLStateVisuals() 
{
	TArray<FInt32Vector2> AOETiles = GameMode->SkillManagerActor->FindSkillAOE(&SelectedCharacter->Skills[SelectedSkillIndex], HoveredTile, SelectedCharacter);
	int i = 0;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (auto tile : AOETiles)
	{
		FVector tileLocation = Grid->GetTileDefinition(tile)->Location;
		if (i < AOESkillTileWidgets.Num())
		{
			AOESkillTileWidgets[i]->SetActorLocation(tileLocation);
			AOESkillTileWidgets[i]->SetActorHiddenInGame(false);
		}
		else
		{
			AActor* widget = GetWorld()->SpawnActor<AActor>(AOESkillTileWidgetsclass, tileLocation, FRotator::ZeroRotator, SpawnInfo);
			AOESkillTileWidgets.Add(widget);
		}
		i++;
	}
}
void APlayerPawn::DestroyFRIENDLYCHARACTERStateVisuals()
{
	for (auto oldTile : WalkingTileWidgets)
	{
		oldTile->SetActorHiddenInGame(true);
	}
}
void APlayerPawn::DestroySKILLStateVisuals()
{
	if(SkillWidget)
		SkillWidget->Destroy();

	for (auto oldTile : AOESkillTileWidgets)
	{
		oldTile->SetActorHiddenInGame(true);
	}
}

void APlayerPawn::UpdateSKILLRangeVisuals()
{
	DestroySKILLRangeVisuals();

	if (SelectedSkillIndex < 0)
		return;

	FSkillDefinition currentSkill = SelectedCharacter->Skills[SelectedSkillIndex];

	if (currentSkill.AOEtype == AOEType::DIRECTIONALAOE)
		return;

	FIntVector2 characterTile = SelectedCharacter->CurrentTile;
	int maxRange = currentSkill.MaxRange;
	int minRange = currentSkill.MinRange;
	int i = 0;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int X = -maxRange; X <= maxRange; X++) 
	{
		for (int Y = -maxRange; Y <= maxRange; Y++)
		{
			FIntVector2 currentTile = characterTile + FIntVector2(X, Y);
			int distance = Grid->CalculateDistance(characterTile, currentTile);
			if (distance >= minRange && distance <= maxRange)
			{
				if (auto currentTileDefinition = Grid->GetTileDefinition(currentTile))
				{
					if (Grid->CheckForObstruction(characterTile,currentTile).bBlockingHit)
						continue;

					auto tileLocation = currentTileDefinition->Location;
					if (i < RangeSkillTileWidgets.Num())
					{
						RangeSkillTileWidgets[i]->SetActorLocation(tileLocation);
						RangeSkillTileWidgets[i]->SetActorHiddenInGame(false);
					}
					else
					{
						AActor* widget = GetWorld()->SpawnActor<AActor>(RangeSkillTileWidgetsclass, tileLocation, FRotator::ZeroRotator, SpawnInfo);
						RangeSkillTileWidgets.Add(widget);
					}
					i++;
				}
			}
		}
	}
}
void APlayerPawn::DestroySKILLRangeVisuals()
{
	for (auto oldTile : RangeSkillTileWidgets)
	{
		oldTile->SetActorHiddenInGame(true);
	}
}

void APlayerPawn::EndTurn()
{
	Interaction2SKILL();
	Interaction2FRIENDLYCHARACTER();
	Interaction2NONE();

	PlayersTurn = false;

	GameMode->GiveTurnToEnemy();
}

void APlayerPawn::OnTurnStart()
{
	Super::OnTurnStart();
	PlayersTurn = true;
}

void APlayerPawn::SetAP(int APAmmount)
{
	Super::SetAP(APAmmount);
	HUDInstance->UpdateAPValues(CurrentAP, MaxAP);
}

void APlayerPawn::UpdateAPValues()
{
	int Characters = GameMode->PlayerCharacters.Num();

	MaxAP = Characters * MaxAPPerUnit;
	APregen = Characters * APPerUnit;
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("CameraX", this, &APlayerPawn::ManageInputCameraX);
	InputComponent->BindAxis("CameraY", this, &APlayerPawn::ManageInputCameraY);
	InputComponent->BindAxis("CameraZoom", this, &APlayerPawn::ManageInputCameraZoom);
	InputComponent->BindAction("Interact1", IE_Pressed, this, &APlayerPawn::ManageInputInteraction1);
	InputComponent->BindAction("Interact2", IE_Pressed, this, &APlayerPawn::ManageInputInteraction2);
	InputComponent->BindAction("EndTurn", IE_Pressed, this, &APlayerPawn::ManageInputEndTurn);
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
	if (!PlayersTurn)
		return;

	switch (SelectionState)
	{
	case PlayerSelectionState::NONE:
		Interaction1NONE();
		break;
	case PlayerSelectionState::FRIENDLYCHARACTER:
		Interaction1FRIENDLYCHARACTER();
		break;
	case PlayerSelectionState::SKILL:
		Interaction1SKILL();
		break;
	default:
		break;
	}
}
void APlayerPawn::Interaction1NONE()
{
	FTileDefinition* hoveredTileDefinition = Grid->GetTileDefinition(HoveredTile);

	if (!hoveredTileDefinition->Occupant)
		return;

	AGameplayCharacter* targetedCharacter = Cast<AGameplayCharacter>(hoveredTileDefinition->Occupant);
	if (targetedCharacter->Faction == Factions::PLAYER)
	{
		SelectedSkillIndex = -1;
		HUDInstance->UpdateSkillListVisuals();

		SelectedCharacter = targetedCharacter;
		HUDInstance->UpdateSkillList(SelectedCharacter->Skills);

		SelectionState = PlayerSelectionState::FRIENDLYCHARACTER;
	}
}
void APlayerPawn::Interaction1FRIENDLYCHARACTER()
{
	FTileDefinition* hoveredTileDefinition = Grid->GetTileDefinition(HoveredTile);
	if (hoveredTileDefinition->Occupant)
		return;

	SelectedCharacter->WalkToTileAsCharacterAsGameplayPawn(HoveredTile, this);

	DestroyFRIENDLYCHARACTERStateVisuals();
}
void APlayerPawn::Interaction1SKILL()
{
	FTileDefinition* hoveredTileDefinition = Grid->GetTileDefinition(HoveredTile);
	FSkillDefinition* skill = &SelectedCharacter->Skills[SelectedSkillIndex];
	SelectedCharacter->UseSkillAsGameplayPawn(skill, HoveredTile, this);
}

void APlayerPawn::ManageInputInteraction2()
{
	if (!PlayersTurn)
		return;

	switch (SelectionState)
	{
	case PlayerSelectionState::NONE:
		Interaction2NONE();
		break;
	case PlayerSelectionState::FRIENDLYCHARACTER:
		Interaction2FRIENDLYCHARACTER();
		break;
	case PlayerSelectionState::SKILL:
		Interaction2SKILL();
		break;
	default:
		break;
	}
}
void APlayerPawn::Interaction2NONE()
{

}
void APlayerPawn::Interaction2FRIENDLYCHARACTER()
{
	TArray<FSkillDefinition> EmptySkills;
	HUDInstance->UpdateSkillList(EmptySkills);
	SelectionState = PlayerSelectionState::NONE;

	DestroyFRIENDLYCHARACTERStateVisuals();
}
void APlayerPawn::Interaction2SKILL()
{
	SelectedSkillIndex = -1;
	HUDInstance->UpdateSkillListVisuals();
	SelectionState = PlayerSelectionState::FRIENDLYCHARACTER;

	UpdateSKILLRangeVisuals();
	DestroySKILLStateVisuals();
}

void APlayerPawn::ManageInputEndTurn() 
{
	if (!PlayersTurn)
		return;

	EndTurn();
}

