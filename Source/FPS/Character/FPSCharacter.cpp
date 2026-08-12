#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

AFPSCharacter::AFPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

    FPBodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPBodyMesh"));
	FPBodyMesh->SetupAttachment(GetCapsuleComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(FPBodyMesh, FName(TEXT("FPCamera")));
}
