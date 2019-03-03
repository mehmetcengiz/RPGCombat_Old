// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Characters/CharacterComponents/AttackingComponents/BowAttackingComponent.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	SWORDANDSHIELD		UMETA(DisplayName = "Sword and Shield"),
	BOW					UMETA(DisplayName = "Bow"),
	MAGE				UMETA(DisplayName = "Mage")
};


UCLASS()
class RPGCOMBAT_API AWeapon : public AActor
{
	GENERATED_BODY()



public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnPrimaryAttack();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Weapon Components")
	class UStaticMeshComponent* WeaponMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Components")
	class USphereComponent* WeaponCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Components")
	FName WeaponAttachingSocketName;	
	




public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Weapon Components")
	TSubclassOf<class UCharacterAttackingComponent> AttackingComponent;

	FName GetWeaponAttachingSocketName() const { return WeaponAttachingSocketName; }

	virtual void OnAttachedToCharacter();
	virtual void OnDetachFromCharacter();

};
