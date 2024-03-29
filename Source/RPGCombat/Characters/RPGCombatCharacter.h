// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "CharacterAnimInterface.h"
#include "Systems/Items/Item.h"

#include "RPGCombatCharacter.generated.h"

class AWeapon;
class UCharacterAttackingComponent;
class UCharEquipmentComponent;

UCLASS()
class RPGCOMBAT_API ARPGCombatCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	UCharEquipmentComponent* CharacterEquipmentComponent = nullptr;



public:
	// Sets default values for this character's properties
	ARPGCombatCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*Controls and Mechanics*/

	/*
	 *Movement actions
	 */
	bool bIsCharacterFocused = false;

	/** Base turn rate, in deg/sec.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;

	void TurnAtRate(float Rate);
	void AddControllerYawInput(float val) override;
	void LookUpAtRate(float Rate);
	void AddControllerPitchInput(float val) override;
	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnFocusedActor();
	void SelectFocusedActor();
	
	UPROPERTY(BlueprintReadWrite, Category = "Character")
	bool bIsFocused;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
	AActor* ActorToFocus;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Debug")
	AActor* FocusActorToDebug;

protected:
	UFUNCTION(BlueprintCallable,Category ="Character Movements")
	void SetFocusActor(AActor* NewActorToFocus) { ActorToFocus = NewActorToFocus; }

	/*
	 *Item actions
	 */
public:
	UPROPERTY(BlueprintReadWrite, Category = "Attacking Component")
	UCharacterAttackingComponent* CharacterAttackingComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	bool bIsEquippedWeapon;

	//UFUNCTION(BlueprintCallable,Category = "Weapon")
	//void EquipWeapon(AWeapon* NewWeapon);

	UFUNCTION(BlueprintCallable,Category = "Weapon")
	void EquipWeapon(FItem NewWeapon);
	
	AWeapon* CurrentWeapon_L = nullptr;
	AWeapon* CurrentWeapon_R = nullptr;

	UFUNCTION(BlueprintCallable,Category = "Attacking Component")
	void SwitchAttackingComponentClass(UClass* NewComponentClass);

	
private:
	ICharacterAnimInterface* CharacterAnimInterface;
	UAnimInstance* CharacterAnimInstance;
	bool bIsImplementsCharacterAnimInterface;

public:
	//Attacking.
	void PrimaryAttackPressed();


	
};
