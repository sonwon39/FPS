#include "FpsWeapon.h"
#include "Settings/FpsWeaponSettings.h"
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

FTransform AFpsWeapon::GetAimPoint_Implementation()
{
	return WeaponMesh->GetSocketTransform(AimPointSocketName, ERelativeTransformSpace::RTS_Component);
}

float AFpsWeapon::GetAimingDuration()
{
	if(WeaponSettings == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponSettings 할당 안됨"));
		return 0.25f;
	}
	return WeaponSettings->AimingDuration;
}