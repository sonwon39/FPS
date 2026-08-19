// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FpsWeapon.generated.h"

class USkeletalMeshComponent;
class UFpsWeaponSettings;

UCLASS()
class FPS_API AFpsWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AFpsWeapon();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintNativeEvent)
	FTransform GetAimPoint();

	float GetAimingDuration();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	FName AimPointSocketName = FName(TEXT("AimPoint"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UFpsWeaponSettings> WeaponSettings;
};
