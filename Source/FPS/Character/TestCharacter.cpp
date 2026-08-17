#include "TestCharacter.h"
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

#include "Animation/AnimMontage.h"

#include "UObject/ConstructorHelpers.h"
#include "Weapon/FpsWeapon.h"

ATestCharacter::ATestCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	USkeletalMeshComponent* BodyMesh = GetMesh();
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -92.0f));
	BodyMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(BodyMesh, FName(TEXT("FPPCamera_socket")));
	SpringArm->TargetArmLength = 600.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) 
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* eic = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (eic == nullptr)	{ return; }

	if (MoveAction)
	{
		eic->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATestCharacter::OnMove);
	}

	if (LookAction)
	{
		eic->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATestCharacter::OnLook);
	}

	if (SprintAction)
	{
		eic->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ATestCharacter::OnSprintStart);
		eic->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATestCharacter::OnSprintStop);
	}

	if (AimAction)
	{
		eic->BindAction(AimAction, ETriggerEvent::Triggered, this, &ATestCharacter::OnAimStart);
		eic->BindAction(AimAction, ETriggerEvent::Completed, this, &ATestCharacter::OnAimStop);
	}

	if (JumpAction)
	{
		eic->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ATestCharacter::OnJumpStart);
		eic->BindAction(AimAction, ETriggerEvent::Completed, this, &ATestCharacter::OnJumpStop);
	}

	if (SwitchViewAction)
	{
		eic->BindAction(SwitchViewAction, ETriggerEvent::Started, this, &ATestCharacter::OnSwitchViewMode);
	}

	if (AttackAction)
	{
		eic->BindAction(AttackAction, ETriggerEvent::Started, this, &ATestCharacter::OnAttack);
	}
}

void ATestCharacter::PossessedBy(AController* NewController)
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

void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATestCharacter::OnMove(const FInputActionValue& Value)
{
	const FVector2D axis = Value.Get<FVector2D>();

	const FRotator yawOnly(0.0f, GetControlRotation().Yaw, 0.0f);
	
	AddMovementInput(yawOnly.RotateVector(FVector::ForwardVector), axis.Y);
	AddMovementInput(yawOnly.RotateVector(FVector::RightVector), axis.X);
}

void ATestCharacter::OnLook(const FInputActionValue& Value)
{
	const FVector2D axis = Value.Get<FVector2D>();

	AddControllerYawInput(axis.X);
	AddControllerPitchInput(-axis.Y);
}

void ATestCharacter::OnSprintStart(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	SetSprinting(true);
}

void ATestCharacter::OnSprintStop(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	SetSprinting(false);
}

void ATestCharacter::OnAimStart(const FInputActionValue& Value)
{
	SetAiming(true);
}

void ATestCharacter::OnAimStop(const FInputActionValue& Value)
{
	SetAiming(false);
}

void ATestCharacter::OnJumpStart(const FInputActionValue& Value)
{
	Jump();
}

void ATestCharacter::OnJumpStop(const FInputActionValue& Value)
{
	StopJumping();
}

void ATestCharacter::OnSwitchViewMode(const FInputActionValue& Value)
{
	if(bFpsMode)
	{
		// tps로 전환
		Camera->AttachToComponent(
			SpringArm, 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale
		);
		Camera->bUsePawnControlRotation = false;
		bFpsMode = false;

		GetMesh()->UnHideBoneByName(FName(TEXT("head")));
	}
	else
	{
		// fps로 전환
		Camera->AttachToComponent(
			GetMesh(), 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			FName(TEXT("FPPCamera_socket"))
		);
		Camera->bUsePawnControlRotation = true;
		bFpsMode = true;
		GetMesh()->HideBoneByName(FName(TEXT("head")), EPhysBodyOp::PBO_None);
	}
}

void ATestCharacter::OnAttack(const FInputActionValue& Value)
{
	PlayAnimMontage(AttackMontage);
}

void ATestCharacter::SetAiming(const bool bNewAiming)
{
	bAiming = bNewAiming;
}


void ATestCharacter::SetSprinting(const bool bNewSprinting)
{
	bSprinting = bNewSprinting;
}