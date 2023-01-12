// Fill out your copyright notice in the Description page of Project Settings.


#include "MyKartPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/InputComponent.h"
// Sets default values
AMyKartPawn::AMyKartPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	MyMovementComponent = CreateDefaultSubobject<UMyActorMovementComponent>(TEXT("MyMovementComponent"));
	MyKartMovementReplicator = CreateDefaultSubobject<UMyKartMovementReplicator>(TEXT("MyKartMovementReplicator"));
}

// Called when the game starts or when spawned
void AMyKartPawn::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) {
		NetUpdateFrequency = 1; // 디폴트 값은 더 작을 것이다.
	}
}



// Called every frame
void AMyKartPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(GetLocalRole()), this, FColor::White, DeltaTime); 
	DrawDebugString(GetWorld(), FVector(0, 0, -200), FString::Printf(TEXT(" %d km/h"), (int)MyMovementComponent->GetVelocity().Size()), this, FColor::White, DeltaTime);

}



FString AMyKartPawn::GetEnumText(ENetRole Role_)
{
	switch (Role_)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "ROLE_SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "ROLE_AutonomousProxy";
	case ROLE_Authority:
		return "ROLE_Authority";
	default:
		return "Error";
	}
}






// Called to bind functionality to input
void AMyKartPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyKartPawn::MoveForward); // W S
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyKartPawn::MoveRight); // A D 
}

void AMyKartPawn::MoveForward(float Value)
{
	if (MyMovementComponent == nullptr) return;
	MyMovementComponent->SetThrottle(Value);
}

void AMyKartPawn::MoveRight(float Value)
{
	if (MyMovementComponent == nullptr) return;
	MyMovementComponent->SetSteeringThrow(Value);
}


