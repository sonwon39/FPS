#include "FpsWeapon.h"
#include "Settings/FpsWeaponSettings.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"

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

UAnimMontage* AFpsWeapon::GetDrawAnimMontage()
{
	if(WeaponSettings)
	{
		return WeaponSettings->DrawMontage;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponSettings is nullptr"));
		return nullptr;

	}
}

void AFpsWeapon::SetOnlyOwnerSee_Implementation(bool bNewOnlyOwnerSee)
{
	if(WeaponMesh)
	{
		WeaponMesh->SetOnlyOwnerSee(bNewOnlyOwnerSee);
	}
}

void AFpsWeapon::SetVisibility_Implementation(bool bNewVisibility)
{
	if(WeaponMesh)
	{
		WeaponMesh->SetVisibility(bNewVisibility);
	}
}