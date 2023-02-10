// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "RecoilSystemCharacter.h"
#include "RecoilSystemProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TimelineComponent.h"
#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{



	CurrentAmmo = DefaultAmmo;
}



void UTP_WeaponComponent::OnFire()
{
	

	AController* Controller = Character->GetController();

	// Try and fire a projectile
	if (CurrentAmmo > 0)
	{
		CurrentAmmo--;
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			FHitResult Hit;
			FVector CameraLocation;
			FRotator CameraRotation;
			Controller->GetPlayerViewPoint(CameraLocation,CameraRotation);

			/*FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);*/

			FVector Muzzle = Character->GetMesh1P()->GetBoneLocation("Muzzle");

			FVector Start = CameraLocation;
			FVector End = Start + CameraRotation.Vector() * 50'000;

			bool bhit = World->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility);

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuuzzleFlash, Muzzle);
		
			if (bhit) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleFX,Hit.ImpactPoint);
				UGameplayStatics::SpawnDecalAtLocation(World, HitDecalMaterial, FVector(15.0f), Hit.Location,Hit.ImpactNormal.Rotation(),10.0f);
			}

			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
			}

			// Try and play a firing animation if specified
			if (FireAnimation != nullptr)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
				if (AnimInstance != nullptr)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}
		}
		
	}
	else {
		OnStopFire();

	}

	// Try and play the sound if specified
	
}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ARecoilSystemCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	FOnTimelineFloat XRecoilCurve;
	FOnTimelineFloat YRecoilCurve;

	XRecoilCurve.BindUFunction(this, FName("StartHorizontalRecoil"));
	YRecoilCurve.BindUFunction(this, FName("StartVerticalRecoil"));
	
	if (!HorizontalCurve || !VerticalCurve)
	{
		return;
	}

	RecoilTimeLine.AddInterpFloat(HorizontalCurve, XRecoilCurve);
	RecoilTimeLine.AddInterpFloat(VerticalCurve, YRecoilCurve);

}

void UTP_WeaponComponent::StartHorizontalRecoil(float Value)
{
	if (RecoilTimeLine.IsReversing())
	{
		return;
	}
	Character->AddControllerYawInput(Value);
}

void UTP_WeaponComponent::StartVerticalRecoil(float Value)
{
	if (RecoilTimeLine.IsReversing())
	{
		return;
	}
	Character->AddControllerPitchInput(Value);
}

void UTP_WeaponComponent::StartRecoil()
{
	RecoilTimeLine.PlayFromStart();
}

void UTP_WeaponComponent::ReverseRecoil()
{
	RecoilTimeLine.ReverseFromEnd();
}

void UTP_WeaponComponent::OnStartFire()
{
	if(CurrentAmmo <= 0){
		return;
	}
	StartRotation = Character->GetControlRotation();

	OnFire();
	StartRecoil();
	GetWorld()->GetTimerManager().SetTimer(AutomaticFireHandle, this, &UTP_WeaponComponent::OnFire, 0.1, true);
}

void UTP_WeaponComponent::OnStopFire()
{
	ReverseRecoil();
	GetWorld()->GetTimerManager().ClearTimer(AutomaticFireHandle);
}

void UTP_WeaponComponent::OnStartReload()
{
	UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, Character->GetActorLocation());
	FTimerHandle ReloadHandle;
	GetWorld()->GetTimerManager().SetTimer(ReloadHandle, this, &UTP_WeaponComponent::Reload, 1.25f, false);
}

void UTP_WeaponComponent::Reload()
{
	CurrentAmmo = DefaultAmmo;

}