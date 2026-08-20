// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FpsWeaponSettings.generated.h"

class UAnimSequence;
class UAnimMontage;

UCLASS()
class FPS_API UFpsWeaponSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", Meta=(ClampMin=1))
	int32 MagazineSize = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AimingDuration = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	TObjectPtr<UAnimSequence> IdleAnimSequence = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	TObjectPtr<UAnimMontage> DrawMontage = nullptr;
};
