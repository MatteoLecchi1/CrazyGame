// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprints/Grid/GridManagerActor.h"
#include "Structs/TileDefinition.h"
#include "Blueprints/Core/CrazyGameInstance.h"
#include "Blueprints/UI/CharacterWidget.h"
#include "Components/WidgetComponent.h"
#include "Blueprints/Core/GameplayPawn.h"
#include "GameplayCharacter.generated.h"

UCLASS()
class CRAZY_API AGameplayCharacter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameplayCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats|Combat")
	int CurrentHP = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStats|Combat")
	int MaxHP = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStats|Combat")
	int Armor = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStats|Combat")
	int MovementSpeed = 0;
	UPROPERTY()
	int CurrentMovement = 0;

	UPROPERTY(VisibleAnywhere, Category = "CharacterStats|Combat")
	int CurrentAP = 0;
	UPROPERTY(EditAnywhere, Category = "CharacterStats|Combat")
	int MaxAP = 2;

	UPROPERTY(EditAnywhere, Category = "CharacterStats|Combat")
	Factions Faction;

	UPROPERTY(EditAnywhere, Category = "CharacterStats|Combat")
	TArray<FName> SkillList;
	UPROPERTY()
	TArray<FSkillDefinition> Skills;

	UPROPERTY(EditAnywhere, Category = "CharacterStats|Combat")
	TMap<DamageElements, int> DamageResistanceMap;

	UPROPERTY(EditAnywhere, Category = "CharacterStats|Combat")
	int PushCollisionDamage;

	UPROPERTY(VisibleAnywhere, Category = "CharacterStats|Values")
	FIntVector2 CurrentTile;
	UPROPERTY()
	AGridManagerActor* Grid;

	UPROPERTY()
	UCharacterWidget* CharacterWidget;

	UPROPERTY()
	AGameplayGameMode* GameMode;

	UPROPERTY(EditAnywhere, Category = "CharacterStats|AI|Reward Weights")
	float APWeight = -5;
	UPROPERTY(EditAnywhere, Category = "CharacterStats|AI|Reward Weights")
	float DamageWeight = 1;
	UPROPERTY(EditAnywhere, Category = "CharacterStats|AI|Reward Weights")
	float FriendlyDamageWeight = -1;
	UPROPERTY(EditAnywhere, Category = "CharacterStats|AI|Reward Weights")
	float RemaningWalkDistanceWeight = 1;
	UPROPERTY(EditAnywhere, Category = "CharacterStats|AI|Reward Weights")
	float FavoredDistanceFromTarget = 1;
	UPROPERTY(EditAnywhere, Category = "CharacterStats|AI|Reward Weights")
	float FavoredDistanceMultiplierPerTile = -3;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Initialize();

	UFUNCTION(BlueprintCallable)
	float MyTakeDamage(float DamageAmount, DamageElements damageElement);
	float CheckInflictedDamage(float DamageAmount, DamageElements damageElement);

	int UseSkill(FSkillDefinition* skillUsed, FIntVector2 targetedTile, int AP);
	void UseSkillAsCharacter(FSkillDefinition* skillUsed, FIntVector2 targetedTile);
	void UseSkillAsGameplayPawn(FSkillDefinition* skillUsed, FIntVector2 targetedTile, AGameplayPawn* Instigator);

	int WalkToTile(FIntVector2 targetedTile, int AP);
	void WalkToTileAsCharacter(FIntVector2 targetedTile);
	void WalkToTileAsGameplayPawn(FIntVector2 targetedTile, AGameplayPawn* Instigator);
	int CheckWalkToTileAPCost(FIntVector2 targetedTile);
	FIntVector2 CheckWalkToTileAPCostAndRemaningWalk(FIntVector2 targetedTile);

	UFUNCTION(BlueprintCallable)
	float PushToTile(FIntVector2 targetedTile);
	float CheckPushToTile(FIntVector2 targetedTile);

	float PlayPushCollisionReaction();

	UFUNCTION(BlueprintCallable)
	void MoveToTile(FIntVector2 targetedTile);


	void AddSkill(FName SkillKey, UCrazyGameInstance* GameInstance);

	void OnTurnStart();
	void FillAP();
	void DecreseAllCooldownsBy(int ammount);

	void OnDeath();
};
