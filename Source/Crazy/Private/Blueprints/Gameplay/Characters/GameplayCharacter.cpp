// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprints/Core/CrazyGameInstance.h"
#include "Blueprints/Player/PlayerPawn.h"
#include "Blueprints/Core/SkillManagerActor.h"

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

	GameMode = Cast<AGameplayGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->AddCharacterToArrays(this);
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
	int damageMultiplier = 0;
	float currentArmor = Armor;
	float MinDamage = 0.f;
	float MaxDamage = 999999.f;

	if (int* damageResistance = DamageResistanceMap.Find(damageElement)) 
	{
		damageMultiplier = *damageResistance;
	}

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
	case DamageElements::MAGIC:
		ignoreArmor = true;
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

	GameMode->DropCharacterFromArrays(this);

	Destroy();
}

int AGameplayCharacter::UseSkill(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, int AP)
{
	if (skillUsed->CurrentCooldown > 0)
		return 0;
	if (skillUsed->APCost > AP)
		return 0;

	int distance = Grid->CalculateDistance(CurrentTile, targetedTile);

	if (distance > skillUsed->MaxRange || distance < skillUsed->MinRange)
		return 0;

	if (Grid->CheckForObstruction(CurrentTile, targetedTile).bBlockingHit)
		return 0;

	GameMode->SkillManagerActor->ManageSkill(skillUsed, targetedTile);

	skillUsed->CurrentCooldown = skillUsed->Cooldown;

	int APUsed = skillUsed->APCost;
	return APUsed;
}
void AGameplayCharacter::UseSkillAsCharacter(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile)
{
	int APused = UseSkill(skillUsed, targetedTile,CurrentAP);
	CurrentAP -= APused;
}
void AGameplayCharacter::UseSkillAsGameplayPawn(FSkillDefinition* skillUsed, FInt32Vector2 targetedTile, AGameplayPawn* myInstigator)
{
	int APused = UseSkill(skillUsed, targetedTile, myInstigator->CurrentAP);
	myInstigator->SetAP(myInstigator->CurrentAP - APused);
}

int AGameplayCharacter::WalkToTile(FInt32Vector2 targetedTile, int AP)
{
  	TArray<FInt32Vector2> path = Grid->FindPath(CurrentTile, targetedTile);

	int distance = path.Num();
	if (distance == 0)
		return 0;

	int MaxDistance = CurrentMovement + MovementSpeed * AP;
	if (MaxDistance == 0)
		return 0;

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

		return APUsed;
	}
	return 0;
}
void AGameplayCharacter::WalkToTileAsCharacter(FInt32Vector2 targetedTile)
{
	int APused = WalkToTile(targetedTile, CurrentAP);
	CurrentAP -= APused;
}
void AGameplayCharacter::WalkToTileAsCharacterAsGameplayPawn(FInt32Vector2 targetedTile, AGameplayPawn* myInstigator)
{
	int APused = WalkToTile(targetedTile, myInstigator->CurrentAP);
	myInstigator->SetAP(myInstigator->CurrentAP - APused);
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
	FillAP();
	DecreseAllCooldownsBy(1);
}
void AGameplayCharacter::FillAP()
{
	CurrentAP = MaxAP;
}
void AGameplayCharacter::DecreseAllCooldownsBy(int ammount)
{
	for (int i = 0; i < Skills.Num(); i++)
	{
		Skills[i].CurrentCooldown -= ammount;
	}
}
