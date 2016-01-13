// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "BatteryCollector.h"
#include "Pickup.h"
#include "BatteryCollectorCharacter.h"
#include "BatteryPickup.h"

//////////////////////////////////////////////////////////////////////////
// ABatteryCollectorCharacter

ABatteryCollectorCharacter::ABatteryCollectorCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Collection Sphere
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->AttachTo(RootComponent);
	CollectionSphere->SetSphereRadius(200);

	// Set Power Level
	InitialPower = 2000;
	CharacterPower = InitialPower;

	// Set speed
	SpeedFactor = 0.75f;
	BaseSpeed = 10;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABatteryCollectorCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Collect", IE_Pressed, this, &ABatteryCollectorCharacter::CollectPickup);

	InputComponent->BindAxis("MoveForward", this, &ABatteryCollectorCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABatteryCollectorCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ABatteryCollectorCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ABatteryCollectorCharacter::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &ABatteryCollectorCharacter::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &ABatteryCollectorCharacter::TouchStopped);
}


void ABatteryCollectorCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void ABatteryCollectorCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void ABatteryCollectorCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABatteryCollectorCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABatteryCollectorCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABatteryCollectorCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ABatteryCollectorCharacter::CollectPickup()
{
	TArray<AActor*> CollectedActors;
	CollectionSphere->GetOverlappingActors(CollectedActors);

	float collectedPower = 0;

	for (int i = 0; i < CollectedActors.Num(); i++)
	{
		APickup* const testPickup = Cast<APickup>(CollectedActors[i]);

		if (testPickup && !testPickup->IsPendingKill() && testPickup->IsActive())
		{
			testPickup->WasCollected();
			ABatteryPickup* const tempBattery = Cast<ABatteryPickup>(testPickup);
			if(tempBattery)
			{
				collectedPower += tempBattery->GetPower();
			}
			testPickup->SetActive(false);
		}
	}

	if (collectedPower > 0)
	{
		UpdatePower(collectedPower);
	}
}

float ABatteryCollectorCharacter::GetInitialPower()
{
	return InitialPower;
}

float ABatteryCollectorCharacter::GetCurrentPower()
{
	return CharacterPower;
}

void ABatteryCollectorCharacter::UpdatePower(float powerChange)
{
	CharacterPower += powerChange;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed + SpeedFactor * CharacterPower;
	PowerChangeEffect();
}