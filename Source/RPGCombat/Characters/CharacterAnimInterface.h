// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*Engine includes.*/
#include "CoreMinimal.h"
#include "UObject/Interface.h"

/*Local includes.*/
#include "Weapons/Weapon.h"

#include "CharacterAnimInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterAnimInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGCOMBAT_API ICharacterAnimInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	/**Varibles*/

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimationInterface")
	void SetForward(float Forward);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimationInterface")
	void SetRight(float Right);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimationInterface")
	void SetWeaponType(EWeaponType WeaponType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimationInterface")
	void SetIsFocused(bool IsFocused);	
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimationInterface")
	void SetIsEquippedWeapon(bool IsEquippedWeapon);
};
