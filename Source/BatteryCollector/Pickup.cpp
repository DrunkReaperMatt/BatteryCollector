// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryCollector.h"
#include "Pickup.h"


// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// All pickups start active
	bIsActive = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	RootComponent = PickupMesh;
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}


bool APickup::IsActive() {
	return bIsActive;
}

void APickup::SetActive(bool NewPickupState) {
	bIsActive = NewPickupState;
}

void APickup::WasCollected_Implementation()
{
	FString PickupDebugString = GetName();
	UE_LOG(LogClass, Log, TEXT("You have picked up %s"), *PickupDebugString);	
}

// Called every frame
void APickup::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

