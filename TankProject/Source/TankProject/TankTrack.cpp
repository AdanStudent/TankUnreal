// Fill out your copyright notice in the Description page of Project Settings.


#include "TankTrack.h"

UTankTrack::UTankTrack()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UTankTrack::BeginPlay()
{
	Super::BeginPlay();

	OnComponentHit.AddDynamic(this, &UTankTrack::OnHit);
}

void UTankTrack::ApplySidewaysForce()
{
	//Calculate the slippage speed
	auto SlippageSpeed = FVector::DotProduct(GetRightVector(), GetComponentVelocity());

	auto DeltaTime = GetWorld()->GetDeltaSeconds();

	//Work-out the required acceleration this frame to correct for
	auto CorrectionAcceleration = -SlippageSpeed / DeltaTime * GetRightVector();

	//Calcualte and apply sideways for (F = m*a)
	auto TankRoot = Cast<UStaticMeshComponent>(GetOwner()->GetRootComponent());
	auto CorrectionForce = (TankRoot->GetMass() * CorrectionAcceleration) / 2; //for 2 tracks

	TankRoot->AddForce(CorrectionForce);
}

void UTankTrack::SetThrottle(float Throttle)
{
	this->CurrentThrottle = FMath::Clamp<float>(this->CurrentThrottle + Throttle, -1, 1);

}

void UTankTrack::DriveTrack()
{
	auto ForceApplied = GetForwardVector() * this->CurrentThrottle * this->TrackMaxDrivingForce;

	auto ForceLocation = GetComponentLocation();
	auto TankRoot = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
	TankRoot->AddForceAtLocation(ForceApplied, ForceLocation);
}

void UTankTrack::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, FVector NormalImpulse, const FHitResult & Hit)
{
	//Drive the tracks
	this->DriveTrack();
	//Apply a sideways force
	this->ApplySidewaysForce();

	CurrentThrottle = 0;

}
