// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryCollector.h"
#include "SpawnVolume.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pickup.h"


// Sets default values
ASpawnVolume::ASpawnVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// create box spawn volume
	WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnPoint"));
	RootComponent = WhereToSpawn;

	SpawnDelayMin = 1;
	SpawnDelayMax = 4.5f;
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetRandomPoint() {
	FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
	FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;

	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
}

void ASpawnVolume::SetSpawningActive(bool ShouldSpawn) {
	if (ShouldSpawn) {
		// set timer
		SpawnDelay = FMath::FRandRange(SpawnDelayMin, SpawnDelayMax);
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnVolume::SpawnPickup, SpawnDelay, false);
	}
	else {
		// clear timer
		GetWorldTimerManager().ClearTimer(SpawnTimer);
	}
}

void ASpawnVolume::SpawnPickup(){

	if (WhatToSpawn != NULL){
		UWorld* const World = GetWorld();

		if (World){
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			FVector SpawnLocation = GetRandomPoint();

			FRotator SpawnRotation;
			SpawnRotation.Yaw = FMath::FRand() * 360.0f;
			SpawnRotation.Pitch = FMath::FRand() * 360.0f;
			SpawnRotation.Roll = FMath::FRand() * 360.0f;

			APickup* const SpawnedPickup = World->SpawnActor<APickup>(WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
			GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnVolume::SpawnPickup, SpawnDelay, false);
		}

	}
}

