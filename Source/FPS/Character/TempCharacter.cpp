#include "TempCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"

#include "UObject/ConstructorHelpers.h"

#include "AlsAnimationInstance.h"          // 프론 복귀 시 AnimationInstance 재캐시
#include "Utility/AlsGameplayTags.h"       // 오버레이/스탠스/게이트/뷰모드 태그
#include "Settings/AlsCharacterSettings.h"
#include "Settings/AlsMovementSettings.h"
#include "Utility/AlsConstants.h"          // Transition 슬롯 이름 (ADS 견착 몽타주)

#include "Weapon/FpsWeapon.h"
#include "Animation/AnimMontage.h"

ATempCharacter::ATempCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	USkeletalMeshComponent* body = GetMesh();

	body->SetVisibility(false);
	body->SetCastHiddenShadow(true);  

    FPBodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPBodyMesh"));
	FPBodyMesh->SetupAttachment(GetCapsuleComponent());
	FPBodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -92.0f));
	FPBodyMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	FPBodyMesh->SetOnlyOwnerSee(true);
	FPBodyMesh->SetCastShadow(false); 
    FPBodyMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength  = 350.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(body, FName(TEXT("head")));
	SpringArm->SetRelativeLocation(FVector(25.0f, 0.0f, 25.0f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(FPBodyMesh, FName(TEXT("FPCamera")));
	Camera->SetUsingAbsoluteRotation(true);
	Camera->bUsePawnControlRotation = true;
	bFpsMode = true;

	if (UCharacterMovementComponent* move = GetCharacterMovement())
	{
		move->bOrientRotationToMovement = false;
		move->JumpZVelocity = 620.0f;
		move->AirControl = 0.32f;
		move->GetNavAgentPropertiesRef().bCanCrouch = true;
	}
}

void ATempCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) 
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* eic = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (eic == nullptr)	{ return; }

	if (MoveAction)
	{
		eic->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATempCharacter::OnMove);
	}

	if (LookAction)
	{
		eic->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATempCharacter::OnLook);
	}

	if (SprintAction)
	{
		eic->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ATempCharacter::OnSprintStart);
		eic->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATempCharacter::OnSprintStop);
	}

}

void ATempCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	APlayerController* pc = Cast<APlayerController>(GetController());
	if(pc == nullptr) return;

	ULocalPlayer* lp = pc->GetLocalPlayer();
	if(lp == nullptr) return;

	UEnhancedInputLocalPlayerSubsystem* subsystem = 
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(lp);
	
	if(subsystem && InputMapping)
	{
		subsystem->AddMappingContext(InputMapping, 0);
	} 
} 

void ATempCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetViewMode(AlsViewModeTags::FirstPerson);
	SetDesiredRotationMode(AlsRotationModeTags::VelocityDirection);
}


USkeletalMeshComponent* ATempCharacter::GetPlayerMesh_Implementation()
{
	return FPBodyMesh;
}

AFpsWeapon* ATempCharacter::GetWeapon_Implementation()
{
	return nullptr;
}

FTransform ATempCharacter::GetAimPoint_Implementation()
{
	return FTransform::Identity;
}

void ATempCharacter::OnMove(const FInputActionValue& Value)
{
	const FVector2D axis = Value.Get<FVector2D>();

	const FRotator yawOnly(0.0f, GetControlRotation().Yaw, 0.0f);
	
	AddMovementInput(yawOnly.RotateVector(FVector::ForwardVector), axis.Y);
	AddMovementInput(yawOnly.RotateVector(FVector::RightVector), axis.X);
}

void ATempCharacter::OnLook(const FInputActionValue& Value)
{
	const FVector2D axis = Value.Get<FVector2D>();

	AddControllerYawInput(axis.X);
	AddControllerPitchInput(-axis.Y);
}

void ATempCharacter::OnSprintStart(const FInputActionValue& Value)
{
	SetDesiredGait(AlsGaitTags::Sprinting);
}

void ATempCharacter::OnSprintStop(const FInputActionValue& Value)
{
	SetDesiredGait(AlsGaitTags::Running);
}
