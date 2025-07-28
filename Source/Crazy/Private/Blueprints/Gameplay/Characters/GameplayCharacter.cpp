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
}

// Called every frame
void AGameplayCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AGameplayCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) 
{
	DamageAmount -= Armor;
	CurrentHP = CurrentHP - DamageAmount;

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

void AGameplayCharacter::UseSkill(FSkillDefinition skillUsed, FInt32Vector2 targetedTile)
{
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
}

void AGameplayCharacter::WalkToTile(FInt32Vector2 targetedTile)
{
	int distance = Grid->CalculateDistance(CurrentTile, targetedTile);

	if (distance > MovementSpeed)
		return;

	FHitResult HitResult = Grid->CheckForObstruction(CurrentTile, targetedTile);
	if (HitResult.bBlockingHit)
		return;

	MoveToTile(targetedTile);
}

void AGameplayCharacter::MoveToTile(FInt32Vector2 targetedTile)
{
	FTileDefinition* currentTileDefinition = Grid->GetTileDefinition(CurrentTile);
	FTileDefinition* targetedTileDefinition = Grid->GetTileDefinition(targetedTile);

	currentTileDefinition->Occupant = nullptr;
	targetedTileDefinition->Occupant = this;

	CurrentTile = targetedTile;
	SetActorLocation(targetedTileDefinition->Location);
}

void AGameplayCharacter::AddSkill(FName SkillKey, UCrazyGameInstance* GameInstance)
{
	Skills.Add(GameInstance->GetSkillFromKey(SkillKey));
}