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
#include "Animation/AnimMontage.h"

AFpsCharacter::AFpsCharacter()
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
		eic->BindAction(AimAction, ETriggerEvent::Started, this, &AFpsCharacter::OnAimStart);
		//eic->BindAction(AimAction, ETriggerEvent::Completed, this, &AFpsCharacter::OnAimStop);
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

	if (SwitchWeaponAction)
	{
		eic->BindAction(SwitchWeaponAction, ETriggerEvent::Started, this, &AFpsCharacter::OnSwitchWeapon);
	}
}

void AFpsCharacter::PawnClientRestart()
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

void AFpsCharacter::BeginPlay()
{
	Super::BeginPlay();

	WeaponSpawnParams.Owner = this;
	WeaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FPAnimInstance = FPBodyMesh->GetAnimInstance();   

    DrawMontageEnded.BindUObject(this, &AFpsCharacter::OnDrawMontageEnded);
    MantleMontageEnded.BindUObject(this, &AFpsCharacter::OnMantleMontageEnded);

	WeaponCount = WeaponClassList.Num();
	if(!WeaponClassList.IsEmpty())
	{
		//SpawnWeapon();
	}

	SetViewMode(AlsViewModeTags::FirstPerson);
	SetDesiredRotationMode(AlsRotationModeTags::VelocityDirection);
}

void AFpsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bTickAiming)
	{
		TickAiming(DeltaSeconds);
	}

	if(bTickMantleCamera)
	{
		TickMantleCamera(DeltaSeconds);
	}
}
void AFpsCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	//  Mantle 동안 카메라가 Capsule을 따라가도록 한다.
	if(bFpsMode && MantleCameraAlpha > 0.f)
	{
		const FVector capsuleAnchor = GetCapsuleComponent()->GetComponentTransform().TransformPosition(MantleCameraOffset);
		OutResult.Location = FMath::Lerp(OutResult.Location, capsuleAnchor,
			MantleCameraAlpha * MantleCameraMaxAlpha);
	}
}

USkeletalMeshComponent* AFpsCharacter::GetPlayerMesh_Implementation()
{
	return FPBodyMesh;
}

AFpsWeapon* AFpsCharacter::GetWeapon_Implementation()
{
	return FPWeapon;
}

FTransform AFpsCharacter::GetAimPoint_Implementation()
{
	if(FPWeapon)
	{
		return FPWeapon->GetAimPoint();
	}
	return FTransform::Identity;
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
	bTickAiming = true;
	SetAiming(!GetAiming());

	if(FPWeapon == nullptr) return;
	FovDuration = FPWeapon->GetAimingDuration();

	if(bAiming)
	{
		TargetFov = AimFov;
	}
	else
	{
		FovDuration *= -1.f;
		TargetFov = DefaultFov;
	}
}

void AFpsCharacter::OnJumpStart(const FInputActionValue& Value)
{
	SetJumping(true);
	Jump();
}

void AFpsCharacter::OnJumpStop(const FInputActionValue& Value)
{
	SetJumping(false);
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
	if(GetLocomotionAction().IsValid() || GetDrawing())
		return;

	FHitResult Result;
	FVector Start = Camera->GetComponentLocation();
	FVector Dir = GetControlRotation().Vector();
	FVector End = Start + 100000.f * Dir;
	
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredActor(FPWeapon);
	
	GetWorld()->LineTraceSingleByChannel(Result, Start, End
	    ,ECollisionChannel::ECC_GameTraceChannel1, params);

	if(Result.bBlockingHit && Result.GetActor())
	{
		UE_LOG(LogTemp, Log, TEXT("Hit ! : %s"), *(Result.GetActor()->GetName()));
		DrawDebugLine(GetWorld(), Start, Result.ImpactPoint, FColor::Red, false, 3.f);
	}
}

void AFpsCharacter::OnSwitchWeapon(const FInputActionValue& Value)
{
	if(WeaponCount == 0)
		return;

	const float axis = Value.Get<float>();
	int32 del = 1;
	if(axis < 0.f)
	{
		del = -1;
	}
	int32 NewIndex = FMath::Clamp(WeaponIndex + del, 0, WeaponCount -1);
	if(NewIndex != WeaponIndex)
	{
		WeaponIndex = NewIndex;
		SpawnWeapon();
	}
}

void AFpsCharacter::SetAiming(const bool bNewAiming)
{
	bAiming = bNewAiming;
}

void AFpsCharacter::SetDrawing(const bool bNewDrawing)
{
	bDrawing = bNewDrawing;
}
void AFpsCharacter::SetJumping(const bool bNewJumping)
{
	bJumping = bNewJumping;
}
void AFpsCharacter::TickAiming(float DeltaTime)
{
	if(CurrentFov == TargetFov)
	{
		bTickAiming = false;
		return;
	}

	AdsAlpha = FMath::Clamp(AdsAlpha + DeltaTime / FovDuration, 0.f, 1.f);
	CurrentFov = FMath::Lerp(DefaultFov, AimFov, AdsAlpha);
	Camera->SetFieldOfView(CurrentFov);
}

void AFpsCharacter::TickMantleCamera(float DeltaTime)
{
	if(MantleCameraAlpha == MantleCameraTargetAlpha)
	{
		bTickMantleCamera = false;
		return;
	}
	MantleCameraAlpha = FMath::FInterpConstantTo(MantleCameraAlpha, MantleCameraTargetAlpha, DeltaTime, MantleCameraBlendSpeed);
}

void AFpsCharacter::OnMantlingStarted_Implementation(const FAlsMantlingParameters& Parameters)
{  
    Super::OnMantlingStarted_Implementation(Parameters);
 	UAlsMantlingSettings* MantlingSettings = SelectMantlingSettings(Parameters.MantlingType);
    UAnimMontage* MantlingMontage = MantlingSettings ? MantlingSettings->Montage : nullptr;
	
	if(MantlingMontage)
	{
		const float StartTime = CalculateMantlingStartTime(MantlingSettings, Parameters.MantlingHeight);

		if(PlayFPMontage(MantlingMontage, MantleMontageEnded) > 0.f)
			FPAnimInstance->Montage_SetPosition(MantlingMontage, StartTime);
	}

	// 현재 카메라의 캡슐-로컬 오프셋을 저장
	MantleCameraOffset = GetCapsuleComponent()->GetComponentTransform().InverseTransformPosition(Camera->GetComponentLocation());
	MantleCameraTargetAlpha = 1.f;
	bTickMantleCamera = true;
}

void AFpsCharacter::OnMantlingEnded_Implementation()
{
    Super::OnMantlingEnded_Implementation();

	// 카메라 위치 소켓 위치로 복구
	MantleCameraTargetAlpha = 0.f;
	bTickMantleCamera = true;
}

void AFpsCharacter::OnDrawMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	SetDrawing(false);
}

void AFpsCharacter::OnMantleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{

}

float AFpsCharacter::PlayFPMontage(UAnimMontage* Montage, FOnMontageEnded MontageEnded)
{	
	if (FPAnimInstance == nullptr || Montage == nullptr) return 0.f;
	const float len = FPAnimInstance->Montage_Play(Montage);	
	if(MontageEnded.IsBound() && len > 0.f)
	{
		FPAnimInstance->Montage_SetEndDelegate(MontageEnded, Montage); 
	}	
	return len;
}

void AFpsCharacter::SpawnWeapon()
{
	if(FPWeapon != nullptr)
		FPWeapon->Destroy();
	if(TPWeapon != nullptr)
		TPWeapon->Destroy();

	FPWeapon = GetWorld()->SpawnActor<AFpsWeapon>(WeaponClassList[WeaponIndex], WeaponSpawnParams);
	TPWeapon = GetWorld()->SpawnActor<AFpsWeapon>(WeaponClassList[WeaponIndex], WeaponSpawnParams);
	FPWeapon->SetOnlyOwnerSee(true);

	if(bFpsMode)
	{
		FPWeapon->SetVisibility(true);
		TPWeapon->SetVisibility(false);
	}
	else
	{
		FPWeapon->SetVisibility(false);
		TPWeapon->SetVisibility(true);
	}

	if(FPWeapon == nullptr || TPWeapon == nullptr) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn weapon"));
		return;
	}
	
	bool FPAttachResult = FPWeapon->AttachToComponent(FPBodyMesh, 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("VB ik_hand_gun_pivot")));
	
	bool TPAttachResult = TPWeapon->AttachToComponent(GetMesh(), 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("VB ik_hand_gun_pivot")));
	
	if(FPAttachResult)
	{
		UAnimMontage* DrawMontage = FPWeapon->GetDrawAnimMontage();
		if(PlayFPMontage(DrawMontage, DrawMontageEnded) > 0.f)	SetDrawing(true);

	}
}