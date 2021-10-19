// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DodgeballProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DodgeballFunctionLibrary.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SightSource = CreateDefaultSubobject<USceneComponent>(FName("Sight Source"));
	SightSource->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Fetch the character currently being controlled by the player
	ACharacter *PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this,0);

	//Look at the player character every frame
	bCanSeePlayer = LookAtActor(PlayerCharacter);
	if (bPreviousCanSeePlayer != bCanSeePlayer)
	{
		if (bCanSeePlayer)
		{
			//Start throwing dodgeballs
			GetWorldTimerManager().SetTimer(ThrowTimerHandle, this, &AEnemyCharacter::ThrowDodgeball, ThrowingInterval, true, ThrowingDelay);
		} 
		else
		{
			//Stop throwing dodgeballs
			GetWorldTimerManager().ClearTimer(ThrowTimerHandle);
		}
	}
	bPreviousCanSeePlayer = bCanSeePlayer;
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AEnemyCharacter::LookAtActor(AActor* TargetActor)
{
	if (TargetActor == nullptr)
	{
		return false;
	}

	const TArray<const AActor*> IgnoreActors = { this, TargetActor };
	if (UDodgeballFunctionLibrary::CanSeeActor(GetWorld(), SightSource->GetComponentLocation(), TargetActor, IgnoreActors))
	{
		FVector Start = GetActorLocation();
		FVector End = TargetActor->GetActorLocation();
	
		// Calculate the necessary rotation for the start point to face the end point
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);

		//Set the enemy's Rotation to that rotation
		SetActorRotation(LookAtRotation);
		return true;
	}

	return false;
}

void AEnemyCharacter::ThrowDodgeball()
{
	if (DodgeballClass == nullptr)
	{
		return;
	}

	FVector ForwardVector = GetActorForwardVector();

	float SpawnDistance = 40.f;

	FVector SpawnLocation = GetActorLocation() + (ForwardVector * SpawnDistance);

	FTransform SpawnTransform = FTransform(GetActorRotation(), SpawnLocation);

	//Create a new dodgeball
	ADodgeballProjectile* Projectile = GetWorld()->SpawnActorDeferred<ADodgeballProjectile>(DodgeballClass, SpawnTransform);
	
	//Change initial speed
	Projectile->GetProjectileMovementComponent()->InitialSpeed = 2200;

	//Spawn the dodgeball
	Projectile->FinishSpawning(SpawnTransform);
}

