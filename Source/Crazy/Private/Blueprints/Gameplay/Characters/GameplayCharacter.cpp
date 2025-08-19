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
		GameMode->PlayerCharacters.Add(this);
		break;
	case Factions::BANDITS:
		GameMode->BanditCharacters.Add(this);
		break;
	case Factions::MONSTERS:
		GameMode->MonsterCharacters.Add(this);
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

float AGameplayCharacter::MyTakeDamage(float DamageAmount, DamageElements damageElement)
{
	bool ignoreArmor = false;
	bool canHeal = false;
	float damageMultiplier = 0.f;
	float currentArmor = Armor;
	float MinDamage = 0.f;
	float MaxDamage = 999999.f;

	switch (damageElement)
	{
	case DamageElements::SLASH:
		break;
	case DamageElements::PIERCE:
		break;
	case DamageElements::BLUNT:
		break;
	case DamageElements::HEAL:
		ignoreArmor = true;
		canHeal = true;
		DamageAmount = -DamageAmount;
		break;
	default:
		break;
	}

	if (ignoreArmor)
	{
		currentArmor = 0;
	}
	if (canHeal)
	{
		MinDamage = -999999.f;
	}

	float PostMultiplierDamage = FMath::Clamp(
		(DamageAmount - currentArmor) * (1 - damageMultiplier / 100),
		MinDamage,
		MaxDamage);

	int totalDamage = FMath::TruncToInt(PostMultiplierDamage);

	CurrentHP = CurrentHP - totalDamage;

	CurrentHP = FMath::Clamp(CurrentHP, 0, MaxHP);

	if(CharacterWidget)
	{
		CharacterWidget->UpdateHPValues(CurrentHP, MaxHP);
	}

	if (CurrentHP <= 0)
	{
		OnDeath();
	}

	return totalDamage;
}

void AGameplayCharacter::OnDeath() 
{
	FTileDefinition* targetedTileDefinition = Grid->GetTileDefinition(CurrentTile);
	targetedTileDefinition->Occupant = nullptr;

	auto GameMode = Cast<AGameplayGameMode>(GetWorld()->GetAuthGameMode());
	switch (Faction)
	{

	case Factions::PLAYER:
		GameMode->PlayerCharacters.Remove(this);
		break;
	case Factions::BANDITS:
		GameMode->BanditCharacters.Remove(this);
		break;
	case Factions::MONSTERS:
		GameMode->MonsterCharacters.Remove(this);
		break;
	default:
		break;
	}

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

	AGameplayCharacter* targetedActor = Cast<AGameplayCharacter>(targetedTileDefinition->Occupant);

	for (auto damageInstance : skillUsed.Damage) 
	{
		targetedActor->MyTakeDamage(damageInstance.DamageAmount, damageInstance.DamageElement);
	}

	InstigatorPawn->SetAP(InstigatorPawn->CurrentAP - skillUsed.APCost);
}

void AGameplayCharacter::WalkToTile(FInt32Vector2 targetedTile, AGameplayPawn* InstigatorPawn)
{
  	TArray<FInt32Vector2> path = Grid->FindPath(CurrentTile, targetedTile);

	int distance = path.Num();
	if (distance == 0)
		return;

	int MaxDistance = CurrentMovement + MovementSpeed * InstigatorPawn->CurrentAP;
	if (MaxDistance == 0)
		return;

	if (distance > MaxDistance)
	{
		distance = MaxDistance;
		targetedTile = path[MaxDistance - 1];
	}

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