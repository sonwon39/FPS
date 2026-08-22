#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "FpsCharacterInterface.h"
#include "TempCharacter.generated.h"

class USkeletalMeshComponent;
class UCameraComponent;
class USpringArmComponent;

class UInputMappingContext;
class UInputAction;

class UAnimInstance;

class AFpsWeapon;

struct FInputActionValue;

UCLASS()
class FPS_API ATempCharacter : public AAlsCharacter, public IFpsCharacterInterface
{
	GENERATED_BODY()

public:
	ATempCharacter();	

public:
	void BeginPlay() override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void PawnClientRestart() override;
	

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
	// ── Input ───────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Input")
	TObjectPtr<UInputMappingContext> InputMapping = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Input")
	TObjectPtr<UInputAction> MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Input")
	TObjectPtr<UInputAction> LookAction = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSCharacter|Input")
	TObjectPtr<UInputAction> SprintAction = nullptr;

protected:
	// ── 입력 콜백함수 ───────────────────────────────────────────────────────
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);

	void OnSprintStart(const FInputActionValue& Value);
	void OnSprintStop(const FInputActionValue& Value);

protected:

	// ── 애님 그래프 + 멀티용 상태 전환  ─────────────────────────────────────────────

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

};
