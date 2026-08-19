#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "FpsCharacterInterface.h"
#include "FpsCharacter.generated.h"

class USkeletalMeshComponent;
class UCameraComponent;
class USpringArmComponent;

class UInputMappingContext;
class UInputAction;

class AFpsWeapon;

struct FInputActionValue;

UCLASS()
class FPS_API AFpsCharacter : public AAlsCharacter, public IFpsCharacterInterface
{
	GENERATED_BODY()

public:
	AFpsCharacter();	

public:
	void BeginPlay() override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void PossessedBy(AController* NewController) override;
	void Tick(float DeltaSeconds) override;

public:
	// ── IFpsCharacterInterface ───────────────────────────────────────────────────────

	USkeletalMeshComponent* GetPlayerMesh_Implementation() override;
	AFpsWeapon* GetWeapon_Implementation() override;
	FTransform GetAimPoint_Implementation() override;

public:
	// ── Components ───────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Components")
	TObjectPtr<USkeletalMeshComponent> FPBodyMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Components")
	TObjectPtr<USpringArmComponent> SpringArm = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Components")
	TObjectPtr<UCameraComponent> Camera = nullptr;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Weapon")
	TArray<TSubclassOf<AFpsWeapon>> WeaponClassList;

	FActorSpawnParameters WeaponSpawnParams;
	int32 WeaponIndex = 0;
	int32 WeaponCount = 0;

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AFpsWeapon> CurrentWeapon = nullptr;

public:
	// ── Input ───────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Input")
	TObjectPtr<UInputMappingContext> InputMapping = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Input")
	TObjectPtr<UInputAction> MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Input")
	TObjectPtr<UInputAction> LookAction = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Input")
	TObjectPtr<UInputAction> SprintAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Input")
	TObjectPtr<UInputAction> AimAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Input")
	TObjectPtr<UInputAction> JumpAction = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Input")
	TObjectPtr<UInputAction> SwitchViewAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Input")
	TObjectPtr<UInputAction> AttackAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Input")
	TObjectPtr<UInputAction> SwitchWeaponAction = nullptr;

protected:
	// ── 입력 콜백함수 ───────────────────────────────────────────────────────
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);

	void OnSprintStart(const FInputActionValue& Value);
	void OnSprintStop(const FInputActionValue& Value);

	void OnAimStart(const FInputActionValue& Value);

	void OnJumpStart(const FInputActionValue& Value);
	void OnJumpStop(const FInputActionValue& Value);

	void OnSwitchViewMode(const FInputActionValue& Value);

	void OnAttack(const FInputActionValue& Value);
	void OnSwitchWeapon(const FInputActionValue& Value);

public:

	void SetAiming(const bool bNewAiming);
	bool GetAiming() const {return bAiming;}

protected:

	// ── 애님 그래프 + 멀티용 상태 전환  ─────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TestCharacter|State")
	bool bAiming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TestCharacter|State")
	bool bSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TestCharacter|State")
	bool bFpsMode = false;

protected:
	// ── Movement  ────────────────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestCharacter|Movement")
	float FpsCrouchMoveSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestCharacter|Movement")
	float FpsWalkSpeed = 400.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestCharacter|Movement")
	float FpsRunSpeed = 800.f;

protected:
	// ── View  ─────────────────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestCharacter|View")
	float DefaultFov = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestCharacter|View")
	float AimFov = 70.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TestCharacter|View")
	float CurrentFov = 90.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TestCharacter|Ads")
	float AdsAlpha = 0.f;

	float FovDuration = 1.f;
	float TargetFov = 90.f;

protected:
	bool bTickAiming = false;
	void TickAiming(float DeltaTime);
};
