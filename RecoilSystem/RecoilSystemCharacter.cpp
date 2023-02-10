// Copyright Epic Games, Inc. All Rights Reserved.

#include "RecoilSystemCharacter.h"
#include "RecoilSystemProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Components/TimelineComponent.h"
#include "TP_WeaponComponent.h"
#include "Kismet/KismetMathLibrary.h"


//////////////////////////////////////////////////////////////////////////
// ARecoilSystemCharacter

ARecoilSystemCharacter::ARecoilSystemCharacter()
{

	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;


	

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	Gun->SetupAttachment(Mesh1P, "hand_r");



	FireComponenet = CreateDefaultSubobject<UTP_WeaponComponent>(TEXT("Fire"));
	
}

void ARecoilSystemCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

//////////////////////////////////////////////////////////////////////////// Input



void ARecoilSystemCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	


	if (FireComponenet->RecoilTimeLine.IsPlaying())
	{
		FireComponenet->RecoilTimeLine.TickTimeline(DeltaSeconds);
	}
	

	if (FireComponenet->RecoilTimeLine.IsReversing())
	{
		if (FMath::Abs(YawInput) > 0 || FMath::Abs(PitchInput) > 0)
		{
			FireComponenet->RecoilTimeLine.Stop();
			return;
		}

		FireComponenet->RecoilTimeLine.TickTimeline(DeltaSeconds);

		FRotator NewRotation = UKismetMathLibrary::RInterpTo(GetControlRotation(), FireComponenet->StartRotation,DeltaSeconds,2.0f);


		Controller->ClientSetRotation(NewRotation);
	}

}

void ARecoilSystemCharacter::OnStartFire()
{
	if (FireComponenet) {
		FireComponenet->OnStartFire();
	}
}

void ARecoilSystemCharacter::OnStopFire()
{
	if (FireComponenet) {
		FireComponenet->OnStopFire();
	}
}

void ARecoilSystemCharacter::OnStartReload()
{
	if (FireComponenet) {
		FireComponenet->OnStartReload();
	}
}

void ARecoilSystemCharacter::Reload()
{
	if (FireComponenet) {
		FireComponenet->Reload();
	}
}
void ARecoilSystemCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ARecoilSystemCharacter::OnStartReload);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ARecoilSystemCharacter::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ARecoilSystemCharacter::OnStopFire);
	// Enable touchscreen input


	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ARecoilSystemCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ARecoilSystemCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ARecoilSystemCharacter::Turn);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &ARecoilSystemCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ARecoilSystemCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ARecoilSystemCharacter::LookUpAtRate);
}
void ARecoilSystemCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ARecoilSystemCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ARecoilSystemCharacter::Turn(float Value)
{
	YawInput = Value;
	AddControllerYawInput(Value);
}

void ARecoilSystemCharacter::LookUp(float Value)
{
	PitchInput = Value;
	AddControllerPitchInput(Value);
}

void ARecoilSystemCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ARecoilSystemCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}