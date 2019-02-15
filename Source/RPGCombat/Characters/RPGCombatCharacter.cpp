// Fill out your copyright notice in the Description page of Project Settings.

#include "RPGCombatCharacter.h"

/*Engine inputs.*/
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "CharacterAnimInterface.h"


#include "Engine/World.h"


// Sets default values
ARPGCombatCharacter::ARPGCombatCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

// Called when the game starts or when spawned
void ARPGCombatCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Get Animation class and check interface.
	CharacterAnimInstance = GetMesh()->GetAnimInstance();
	if(CharacterAnimInstance){
		bIsImplementsCharacterAnimInterface = CharacterAnimInstance->GetClass()->ImplementsInterface(UCharacterAnimInterface::StaticClass());
	}
	
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARPGCombatCharacter::BeginOverlap);
}

// Called every frame
void ARPGCombatCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(ActorToFocus){
		TurnFocusedActor();
	}
}

// Called to bind functionality to input
void ARPGCombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ARPGCombatCharacter::PrimaryAttackPressed);
	PlayerInputComponent->BindAction("Focus", IE_Pressed, this, &ARPGCombatCharacter::SelectFocusedActor);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARPGCombatCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARPGCombatCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &ARPGCombatCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARPGCombatCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ARPGCombatCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARPGCombatCharacter::LookUpAtRate);
}

void ARPGCombatCharacter::TurnAtRate(float Rate) {
	// calculate delta for this frame from the rate information
	if(!bIsFocused) {
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
	
}

void ARPGCombatCharacter::AddControllerYawInput(float val) {
	if(!bIsFocused) {
		Super::AddControllerYawInput(val);
	}
	
}

void ARPGCombatCharacter::LookUpAtRate(float Rate) {
	// calculate delta for this frame from the rate information
	if(!bIsFocused) {
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void ARPGCombatCharacter::AddControllerPitchInput(float val) {
	if (!bIsFocused) {
		Super::AddControllerPitchInput(val);
	}
}

void ARPGCombatCharacter::MoveForward(float Value) {
	if ((Controller != NULL) && (Value != 0.0f)){
		
		FVector Direction;
		if (ActorToFocus) {
			Direction = GetActorForwardVector();
			
		}
		else {
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		}

		AddMovementInput(Direction, Value);
	}
	//Calling animation Interface.
	if (bIsImplementsCharacterAnimInterface) {
			ICharacterAnimInterface::Execute_SetForward(CharacterAnimInstance, Value); //Calling blueprint interface.
	}

}

void ARPGCombatCharacter::MoveRight(float Value) {
	if ((Controller != NULL) && (Value != 0.0f)){
		FVector Direction;

		if (ActorToFocus) {
			Direction = GetActorRightVector();
		}
		else {
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			// get right vector 
			Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		}
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}

	//Calling animation Interface.

	if (bIsImplementsCharacterAnimInterface) {
			ICharacterAnimInterface::Execute_SetRight(CharacterAnimInstance, Value); //Calling blueprint interface.
	}

}

void ARPGCombatCharacter::SwitchWeapon(AWeapon* NewWeapon) {
	if (!NewWeapon){
		bIsEquippedWeapon = false;
		return;
	}

	bIsEquippedWeapon = true;
	//Calling animation Interface.
	if(bIsImplementsCharacterAnimInterface) {
		ICharacterAnimInterface::Execute_SetWeaponType(CharacterAnimInstance, NewWeapon->WeaponType); //Calling blueprint interface.
		ICharacterAnimInterface::Execute_SetIsEquippedWeapon(CharacterAnimInstance, bIsEquippedWeapon); //Calling blueprint interface.
	}

	Weapon = NewWeapon;
}


void ARPGCombatCharacter::PrimaryAttackPressed() {
	UE_LOG(LogTemp, Warning, TEXT("RPGCombatCharacter -> PrimaryAttack"));
}

void ARPGCombatCharacter::TurnFocusedActor(){
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), ActorToFocus->GetActorLocation());
	SetActorRotation(NewRotation);

	FRotator NewCameraRotation = UKismetMathLibrary::FindLookAtRotation(FollowCamera->GetComponentLocation(), ActorToFocus->GetActorLocation());
	FollowCamera->SetWorldRotation(FMath::Lerp(FollowCamera->GetComponentRotation(),NewCameraRotation,0.1f));
}

void ARPGCombatCharacter::SelectFocusedActor() {
	if(!bIsFocused) {
		SetFocusActor(FocusActorToDebug);
		bIsFocused = true;
		CameraBoom->bUsePawnControlRotation = false;
	}else {
		SetFocusActor(nullptr);
		bIsFocused = false;
		CameraBoom->bUsePawnControlRotation = true;
	}

	if (bIsImplementsCharacterAnimInterface) {
		ICharacterAnimInterface::Execute_SetIsFocused(CharacterAnimInstance, bIsFocused);
	}
}

void ARPGCombatCharacter::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){
	
	AWeapon* NewWeapon = Cast<AWeapon>(OtherActor);
	if(NewWeapon){
		this->SwitchWeapon(NewWeapon);
	}


}
