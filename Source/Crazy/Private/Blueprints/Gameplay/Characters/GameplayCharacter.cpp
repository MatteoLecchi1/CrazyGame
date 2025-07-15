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

	Grid = Cast<AGridManagerActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManagerActor::StaticClass()));
	int newTile = Grid->GetTileAtLocation(GetActorLocation());

	if (newTile < 0) //invalid tile
		return;

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
	Grid->Tiles[CurrentTile].Occupant = nullptr;
	Destroy();
}

void AGameplayCharacter::UseSkill(FSkillDefinition skillUsed, int targetedTile)
{
	if (!Grid->Tiles[targetedTile].Occupant)
		return;

	int distance = Grid->CalculateDistance(CurrentTile, targetedTile);

	if (distance > skillUsed.MaxRange || distance < skillUsed.MinRange)
		return;

	AActor* targetedActor = Grid->Tiles[targetedTile].Occupant;

	for (auto damageInstance : skillUsed.Damage) 
	{
		UGameplayStatics::ApplyDamage(targetedActor, damageInstance.DamageAmmount, UGameplayStatics::GetPlayerController(GetWorld(), 0), this, nullptr);
	}
}

void AGameplayCharacter::WalkToTile(int targetedTile)
{
	int distance = Grid->CalculateDistance(CurrentTile, targetedTile);

	if (distance > MovementSpeed)
		return;

	MoveToTile(targetedTile);
}

void AGameplayCharacter::MoveToTile(int targetedTile)
{
	Grid->Tiles[CurrentTile].Occupant = nullptr;
	Grid->Tiles[targetedTile].Occupant = this;

	CurrentTile = targetedTile;
	SetActorLocation(Grid->Tiles[CurrentTile].Location);
}

void AGameplayCharacter::AddSkill(FName SkillKey, UCrazyGameInstance* GameInstance)
{
	Skills.Add(GameInstance->GetSkillFromKey(SkillKey));
}