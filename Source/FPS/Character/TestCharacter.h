// FPPMelee 테스트용

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TestCharacter.generated.h"

class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;

class AFpsWeapon;
struct FInputActionValue;

UCLASS()
class FPS_API ATestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATestCharacter();	

public:
	void BeginPlay() override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void PossessedBy(AController* NewController) override;

protected:

public:
	// ── Components ───────────────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components")
	TObjectPtr<USpringArmComponent> SpringArm = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components")
	TObjectPtr<UCameraComponent> Camera = nullptr;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Weapon")
	TObjectPtr<AFpsWeapon> CurrentWeapon = nullptr;

public:
	// ── Input ───────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCharacter|Input")
	TObjectPtr<UInputMappingContext> InputMapping = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCharacter|Input")
	TObjectPtr<UInputAction> MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCharacter|Input")
	TObjectPtr<UInputAction> LookAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCharacter|Input")
	TObjectPtr<UInputAction> SprintAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCharacter|Input")
	TObjectPtr<UInputAction> AimAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCharacter|Input")
	TObjectPtr<UInputAction> JumpAction = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCharacter|Input")
	TObjectPtr<UInputAction> SwitchViewAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestCharacter|Input")
	TObjectPtr<UInputAction> AttackAction = nullptr;

protected:
	// ── 입력 콜백함수 ───────────────────────────────────────────────────────
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);

	void OnSprintStart(const FInputActionValue& Value);
	void OnSprintStop(const FInputActionValue& Value);

	void OnAimStart(const FInputActionValue& Value);
	void OnAimStop(const FInputActionValue& Value);

	void OnJumpStart(const FInputActionValue& Value);
	void OnJumpStop(const FInputActionValue& Value);

	void OnSwitchViewMode(const FInputActionValue& Value);

	void OnAttack(const FInputActionValue& Value);

protected:
	// ── Anim Montage ───────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> AttackMontage;

public:

	void SetAiming(const bool bNewAiming);
	void SetSprinting(const bool bNewSprinting);

protected:

	// ── 애님 그래프 + 멀티용 상태 전환  ───────────────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TestCharacter|State")
	bool bAiming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TestCharacter|State")
	bool bSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TestCharacter|State")
	bool bFpsMode = false;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestCharacter|Movement")
	float WalkSpeed = 400.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestCharacter|Movement")
	float SprintSpeed = 800.f;
};
