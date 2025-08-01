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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int CurrentHP = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxHP = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Armor = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MovementSpeed = 0;
	UPROPERTY()
	int CurrentMovement = 0;

	UPROPERTY(EditAnywhere)
	Factions Faction;

	UPROPERTY(EditAnywhere)
	TArray<FName> SkillList;
	UPROPERTY()
	TArray<FSkillDefinition> Skills;

	UPROPERTY(EditAnywhere)
	TMap<DamageElements, int> DamageResistanceMap;

	UPROPERTY()
	FInt32Vector2 CurrentTile;
	UPROPERTY()
	AGridManagerActor* Grid;

	UPROPERTY()
	UCharacterWidget* CharacterWidget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Initialize();

	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser)override;

	void UseSkill(FSkillDefinition skillUsed, FInt32Vector2 targetedTile, AGameplayPawn* InstigatorPawn);
	void WalkToTile(FInt32Vector2 targetedTile, AGameplayPawn* InstigatorPawn);
	void MoveToTile(FInt32Vector2 targetedTile);

	void AddSkill(FName SkillKey, UCrazyGameInstance* GameInstance);
	void OnTurnStart();
	void OnDeath();
};
