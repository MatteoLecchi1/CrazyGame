// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprints/Core/CrazyGameInstance.h"

// Sets default values
AGameplayCharacter::AGameplayCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameplayCharacter::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
}
void AGameplayCharacter::Initialize()
{
	Grid = Cast<AGridManagerActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManagerActor::StaticClass()));
	FInt32Vector2 newTile = Grid->GetTileAtLocation(GetActorLocation());

	if (newTile.X < 0) //invalid tile
	{
		Destroy();
		return;
	}

	MoveToTile(newTile);

	CurrentHP = MaxHP;

	auto GameInstance = UCrazyGameInstance::GetGameInstance(GetWorld());

	for (auto key : SkillList)
	{
		AddSkill(key, GameInstance);
	}

	auto widget = Cast<UWidgetComponent>(GetComponentByClass<UWidgetComponent>());
	CharacterWidget = Cast<UCharacterWidget>(widget->GetWidget());
	CharacterWidget->UpdateHPValues(CurrentHP, MaxHP);

	AGameplayGameMode* GameMode = Cast<AGameplayGameMode>(GetWorld()->GetAuthGameMode());
	switch (Faction)
	{

	case Factions::PLAYER:
		GameMode->PlayersCharacters.Add(this);
		break;

	default:
		break;
	}
}

// Called every frame
void AGameplayCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AGameplayCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) 
{

	float PostMultiplierDamage = FMath::Clamp(
		(DamageAmount - Armor) * 1, 
		0,
		99999.f);//fix this shit

	int totalDamage = FMath::TruncToInt(PostMultiplierDamage);

	CurrentHP = CurrentHP - totalDamage;
	if(CharacterWidget)
	{
		CharacterWidget->UpdateHPValues(CurrentHP, MaxHP);
	}

	if (CurrentHP <= 0)
	{
		OnDeath();
	}

	return DamageAmount;
}

void AGameplayCharacter::OnDeath() 
{
	FTileDefinition* targetedTileDefinition = Grid->GetTileDefinition(CurrentTile);
	targetedTileDefinition->Occupant = nullptr;
	Destroy();
}

void AGameplayCharacter::UseSkill(FSkillDefinition skillUsed, FInt32Vector2 targetedTile, AGameplayPawn* InstigatorPawn)
{
	if (skillUsed.APCost > InstigatorPawn->CurrentAP)
		return;

	FTileDefinition* targetedTileDefinition = Grid->GetTileDefinition(targetedTile);

	if (!targetedTileDefinition->Occupant)
		return;

	int distance = Grid->CalculateDistance(CurrentTile, targetedTile);

	if (distance > skillUsed.MaxRange || distance < skillUsed.MinRange)
		return;

	if (Grid->CheckForObstruction(CurrentTile, targetedTile).bBlockingHit)
		return;

	AActor* targetedActor = targetedTileDefinition->Occupant;

	for (auto damageInstance : skillUsed.Damage) 
	{
		UGameplayStatics::ApplyDamage(targetedActor, damageInstance.DamageAmount, UGameplayStatics::GetPlayerController(GetWorld(), 0), this, nullptr);
	}

	InstigatorPawn->SetAP(InstigatorPawn->CurrentAP - skillUsed.APCost);
}

void AGameplayCharacter::WalkToTile(FInt32Vector2 targetedTile, AGameplayPawn* InstigatorPawn)
{
	TArray<FInt32Vector2> path = Grid->FindPath(CurrentTile, targetedTile);

	int distance = path.Num();
	if (distance <= 0)
		return;

	for (FInt32Vector2 tile : path)
	{
		FTileDefinition* tileDefinition = Grid->GetTileDefinition(tile);
		DrawDebugSphere(GetWorld(), tileDefinition->Location, 40, 10, FColor::Blue, false, 1);
	}

	if (distance > CurrentMovement + MovementSpeed * InstigatorPawn->CurrentAP)
		return;

	MoveToTile(targetedTile);
	
	CurrentMovement -= distance;
	if(CurrentMovement < 0)
	{
		int APUsed = FMath::DivideAndRoundUp(abs(CurrentMovement), MovementSpeed);
		CurrentMovement += MovementSpeed * APUsed;
		InstigatorPawn->SetAP(InstigatorPawn->CurrentAP - APUsed);
	}
}

void AGameplayCharacter::MoveToTile(FInt32Vector2 targetedTile)
{
	FTileDefinition* currentTileDefinition = Grid->GetTileDefinition(CurrentTile);
	FTileDefinition* targetedTileDefinition = Grid->GetTileDefinition(targetedTile);

	if(currentTileDefinition)
		currentTileDefinition->Occupant = nullptr;
	targetedTileDefinition->Occupant = this;

	CurrentTile = targetedTile;
	SetActorLocation(targetedTileDefinition->Location);
}

void AGameplayCharacter::AddSkill(FName SkillKey, UCrazyGameInstance* GameInstance)
{
	Skills.Add(GameInstance->GetSkillFromKey(SkillKey));
}

void AGameplayCharacter::OnTurnStart()
{
	CurrentMovement = MovementSpeed;
}