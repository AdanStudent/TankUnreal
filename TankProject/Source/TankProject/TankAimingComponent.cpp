// Fill out your copyright notice in the Description page of Project Settings.

#include "TankAimingComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "TankBarrel.h"
#include "TankTurret.h"
#include "Projectile.h"


// Sets default values for this component's properties
UTankAimingComponent::UTankAimingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UTankAimingComponent::Initialize(UTankBarrel * BarrelToSet, UTankTurret * TurretToSet)
{
	Barrel = BarrelToSet;
	Turret = TurretToSet;

}

// Called when the game starts
void UTankAimingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTankAimingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTankAimingComponent::AimAt(FVector HitLocation)
{
	if (!ensure(Barrel))
	{
		return;
	}


	FVector OutLaunchVelocity;
	FVector StartLocation = Barrel->GetSocketLocation(FName("Projectile"));

	//calculate the OutLaunchVelocity
	bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity(this, OutLaunchVelocity, StartLocation, HitLocation, LaunchSpeed, false, 0, 0, ESuggestProjVelocityTraceOption::DoNotTrace);

	if (bHaveAimSolution)
	{
		auto AimDirection = OutLaunchVelocity.GetSafeNormal();
		MoveBarrelTowards(AimDirection);
	}

}

void UTankAimingComponent::Fire()
{

	if (!ensure(Barrel) && ProjectileBlueprint)
	{
		return;
	}

	bool isReloaded = (FPlatformTime::Seconds() - LastFireTime) > ReloadTimeInSeconds;

	if (isReloaded)
	{
		//spawn projectile at barrel location
		auto Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileBlueprint, Barrel->GetSocketLocation(FName("Projectile")), Barrel->GetSocketRotation(FName("Projectile")));

		Projectile->LaunchProjectile(LaunchSpeed);
		LastFireTime = FPlatformTime::Seconds();
	}
}

void UTankAimingComponent::MoveBarrelTowards(FVector AimDirection)
{

	if (!ensure(Barrel) || !ensure(Turret))
	{
		return;
	}

	auto BarrelRotation = Barrel->GetForwardVector().Rotation();
	auto AimAsRotator = AimDirection.Rotation();

	auto DeltaRotator = AimAsRotator - BarrelRotation;

	Barrel->Elevate(DeltaRotator.Pitch);
	Turret->RotateTurret(DeltaRotator.Yaw);

}

