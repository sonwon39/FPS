#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "FPSCharacter.generated.h"

class USkeletalMeshComponent;
class UCameraComponent;

UCLASS()
class FPS_API AFPSCharacter : public AAlsCharacter
{
	GENERATED_BODY()

public:
	AFPSCharacter();	

public:
	// ── 컴포넌트 ───────────────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPSCharacer|Components")
	TObjectPtr<USkeletalMeshComponent> FPBodyMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPSCharacer|Components")
	TObjectPtr<UCameraComponent> Camera = nullptr;

};
