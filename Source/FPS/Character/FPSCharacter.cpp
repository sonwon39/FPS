#include "FpsCharacter.h"
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

AFpsCharacter::AFpsCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	USkeletalMeshComponent* body = GetMesh();

    FPBodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPBodyMesh"));
	FPBodyMesh->SetupAttachment(GetCapsuleComponent());
	FPBodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -92.0f));
	FPBodyMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	FPBodyMesh->SetOnlyOwnerSee(true);
	FPBodyMesh->SetVisibility(false);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength  = 600.f;
	SpringArm->SetupAttachment(body, FName(TEXT("head")));
	SpringArm->bUsePawnControlRotation = true;
	//SpringArm->SetUsingAbsoluteRotation(true);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	//Camera->SetUsingAbsoluteRotation(true);

	if (UCharacterMovementComponent* move = GetCharacterMovement())
	{
		move->bOrientRotationToMovement = false;
		move->JumpZVelocity = 620.0f;
		move->AirControl = 0.32f;
		move->GetNavAgentPropertiesRef().bCanCrouch = true;
	}
}

void AFpsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) 
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* eic = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (eic == nullptr)	{ return; }

	if (MoveAction)
	{
		eic->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFpsCharacter::OnMove);
	}

	if (LookAction)
	{
		eic->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFpsCharacter::OnLook);
	}

	if (SprintAction)
	{
		eic->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AFpsCharacter::OnSprintStart);
		eic->BindAction(SprintAction, ETriggerEvent::Completed, this, &AFpsCharacter::OnSprintStop);
	}

	if (AimAction)
	{
		eic->BindAction(AimAction, ETriggerEvent::Triggered, this, &AFpsCharacter::OnAimStart);
		eic->BindAction(AimAction, ETriggerEvent::Completed, this, &AFpsCharacter::OnAimStop);
	}

	if (JumpAction)
	{
		eic->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AFpsCharacter::OnJumpStart);
		eic->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFpsCharacter::OnJumpStop);
	}

	if (SwitchViewAction)
	{
		eic->BindAction(SwitchViewAction, ETriggerEvent::Started, this, &AFpsCharacter::OnSwitchViewMode);
	}

	if (AttackAction)
	{
		eic->BindAction(AttackAction, ETriggerEvent::Started, this, &AFpsCharacter::OnAttack);
	}
}

void AFpsCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APlayerController* pc = Cast<APlayerController>(NewController);
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

void AFpsCharacter::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters params;
	params.Owner = this;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FPBodyMesh->HideBoneByName(FName(TEXT("head")), EPhysBodyOp::PBO_None);

	if(ViperWeaponClass)
	{
		ViperWeapon = GetWorld()->SpawnActor<AFpsWeapon>(ViperWeaponClass, params);
		ViperWeapon->AttachToComponent(FPBodyMesh, 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("VB ik_hand_gun_pivot")));
	}

	SetViewMode(AlsViewModeTags::FirstPerson);
	SetDesiredRotationMode(AlsRotationModeTags::VelocityDirection);
}

void AFpsCharacter::OnMove(const FInputActionValue& Value)
{
	const FVector2D axis = Value.Get<FVector2D>();

	const FRotator yawOnly(0.0f, GetControlRotation().Yaw, 0.0f);
	
	AddMovementInput(yawOnly.RotateVector(FVector::ForwardVector), axis.Y);
	AddMovementInput(yawOnly.RotateVector(FVector::RightVector), axis.X);
}

void AFpsCharacter::OnLook(const FInputActionValue& Value)
{
	const FVector2D axis = Value.Get<FVector2D>();

	AddControllerYawInput(axis.X);
	AddControllerPitchInput(-axis.Y);
}

void AFpsCharacter::OnSprintStart(const FInputActionValue& Value)
{
	SetDesiredGait(AlsGaitTags::Sprinting);
}

void AFpsCharacter::OnSprintStop(const FInputActionValue& Value)
{
	SetDesiredGait(AlsGaitTags::Running);
}

void AFpsCharacter::OnAimStart(const FInputActionValue& Value)
{
	SetAiming(true);
}

void AFpsCharacter::OnAimStop(const FInputActionValue& Value)
{
	SetAiming(false);
}

void AFpsCharacter::OnJumpStart(const FInputActionValue& Value)
{
	Jump();
}

void AFpsCharacter::OnJumpStop(const FInputActionValue& Value)
{
	StopJumping();
}

void AFpsCharacter::OnSwitchViewMode(const FInputActionValue& Value)
{
	if(bFpsMode)
	{
		// TPS로 전환
		bFpsMode = false;
		
		FPBodyMesh->SetVisibility(false);
		GetMesh()->SetVisibility(true);
		
		Camera->AttachToComponent(SpringArm, 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		Camera->bUsePawnControlRotation = false;
		Camera->SetUsingAbsoluteRotation(false);

	}
	else
	{
		// FPS로 전환
		bFpsMode = true;
		
		FPBodyMesh->SetVisibility(true);
		GetMesh()->SetVisibility(false);

		Camera->AttachToComponent(FPBodyMesh, 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("FPCamera")));
		Camera->bUsePawnControlRotation = true;
		Camera->SetUsingAbsoluteRotation(true);
	}
}

void AFpsCharacter::OnAttack(const FInputActionValue& Value)
{

}

void AFpsCharacter::SetAiming(const bool bNewAiming)
{
	bAiming = bNewAiming;
}