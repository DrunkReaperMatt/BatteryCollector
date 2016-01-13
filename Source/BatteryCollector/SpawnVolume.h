// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class BATTERYCOLLECTOR_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpawnVolume();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	FORCEINLINE class UBoxComponent*  GetWhereToSpawn() const { return WhereToSpawn; }

	UFUNCTION(BlueprintPure, Category = "Spawning")
		FVector GetRandomPoint();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SetSpawningActive(bool ShouldSpawn);

protected:
	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class APickup> WhatToSpawn;

	FTimerHandle SpawnTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		float SpawnDelayMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		float SpawnDelayMax;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* WhereToSpawn;

	void SpawnPickup();

	float SpawnDelay;
};
