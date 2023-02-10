// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "TP_WeaponComponent.generated.h"



class ARecoilSystemCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RECOILSYSTEM_API UTP_WeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* ReloadSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UParticleSystem* MuuzzleFlash;

	UTP_WeaponComponent();


	UFUNCTION(BlueprintCallable, Category="Weapon")
	void OnStartFire();
	void OnStopFire();
	void OnStartReload();
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void OnFire();

	FTimerHandle AutomaticFireHandle;

	int32 CurrentAmmo;

	int32 DefaultAmmo = 30;

	FRotator StartRotation;

	FTimeline RecoilTimeLine;

	virtual void BeginPlay() override;

	UFUNCTION()
		void StartHorizontalRecoil(float Value);

	UFUNCTION()
		void StartVerticalRecoil(float Value);

	void StartRecoil();

	void ReverseRecoil();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
		class UCurveFloat* VerticalCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
		class UCurveFloat* HorizontalCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
		class UParticleSystem* ParticleFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
		class UMaterialInterface* HitDecalMaterial;
protected:

	/** Ends gameplay for this component. */


private:
	/** The Character holding this weapon*/
	ARecoilSystemCharacter* Character;
};
