// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprints/Core/GameplayGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprints/Player/PlayerPawn.h"
#include "Blueprints/Gameplay/Characters/GameplayCharacter.h"
#include "Blueprints/AI/EnemyPawn.h"
#include "Blueprints/Core/SkillManagerActor.h"


AGameplayGameMode::AGameplayGameMode()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AGameplayGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Tick1Timer == 0) 
	{
		
		AActor* Grid = UGameplayStatics::GetActorOfClass(GetWorld(), AGridManagerActor::StaticClass());
		AGridManagerActor* gameplayGrid = Cast<AGridManagerActor>(Grid);
		gameplayGrid->Initialize();

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SkillManagerActor = GetWorld()->SpawnActor<ASkillManagerActor>();
		SkillManagerActor->GameMode = this;
		SkillManagerActor->Grid = gameplayGrid;

		TArray<AActor*> pawns;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(),AGameplayPawn::StaticClass(),pawns);
		for (AActor* pawn : pawns) 
		{
			if (AGameplayPawn* gameplayPawn = Cast<AGameplayPawn>(pawn))
			{
				gameplayPawn->Initialize();
			}
		}

		TArray<AActor*> Characters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameplayCharacter::StaticClass(), Characters);
		for (AActor* Character : Characters)
		{
			if (AGameplayCharacter* gameplayCharacter = Cast<AGameplayCharacter>(Character))
			{
				gameplayCharacter->Initialize();
			}
		}
		Tick1Timer = 1;
	}
}

void AGameplayGameMode::GiveTurnToPlayer()
{
	for (auto character : PlayerCharacters) {
		character->OnTurnStart();
	}

	PlayerPawn->OnTurnStart();
}
void AGameplayGameMode::GiveTurnToEnemy()
{
	for (auto character : BanditCharacters) {
		character->OnTurnStart();
	}

	EnemyPawn->OnTurnStart();
}

void AGameplayGameMode::AddCharacterToArrays(class AGameplayCharacter* Character) 
{
	switch (Character->Faction)
	{

	case Factions::PLAYER:
		PlayerCharacters.Add(Character);
		PlayerPawn->UpdateAPValues();
		break;
	case Factions::BANDITS:
		BanditCharacters.Add(Character);
		break;
	case Factions::MONSTERS:
		MonsterCharacters.Add(Character);
		break;
	default:
		break;
	}
}

void AGameplayGameMode::DropCharacterFromArrays(class AGameplayCharacter* Character)
{
	switch (Character->Faction)
	{

	case Factions::PLAYER:
		PlayerCharacters.Remove(Character);
		PlayerPawn->UpdateAPValues();
		break;
	case Factions::BANDITS:
		BanditCharacters.Remove(Character);
		break;
	case Factions::MONSTERS:
		MonsterCharacters.Remove(Character);
		break;
	default:
		break;
	}
}