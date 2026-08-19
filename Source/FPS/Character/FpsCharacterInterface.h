#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FpsCharacterInterface.generated.h"

class USkeletalMeshComponent;

UINTERFACE(MinimalAPI, Blueprintable)
class UFpsCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

class FPS_API IFpsCharacterInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FpsCharacter")
	USkeletalMeshComponent* GetPlayerMesh();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FpsCharacter")
	AFpsWeapon* GetWeapon();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FpsCharacter")
	FTransform GetAimPoint();
};