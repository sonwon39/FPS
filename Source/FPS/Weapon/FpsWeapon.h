// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FpsWeapon.generated.h"

class USkeletalMeshComponent;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh = nullptr;
};
