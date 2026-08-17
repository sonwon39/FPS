// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FpsWeaponSettings.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API UFpsWeaponSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", Meta=(ClampMin=1))
	int32 MagazineSize = 30;

};
