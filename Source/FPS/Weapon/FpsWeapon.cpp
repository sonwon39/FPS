#include "FpsWeapon.h"

#include "Components/SkeletalMeshComponent.h"

AFpsWeapon::AFpsWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	SetRootComponent(WeaponMesh);
}

void AFpsWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AFpsWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


