// Fill out your copyright notice in the Description page of Project Settings.


#include "MyKartPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Components/InputComponent.h"
// Sets default values
AMyKartPawn::AMyKartPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyKartPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyKartPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 힘이 최대 추진력이 되도록 설정
	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle; // 힘 = 방향 * 최대추진력 * 조절력
	FVector Acceleration = Force / Mass; // a 구하기
	Velocity = Velocity + Acceleration * DeltaTime; // 속도를 얻기 위해서는 기존 속도 + 속도의 변화(dt)

	UpdateRotationFromFQuat(DeltaTime);
	

	UpdateLocationFromVelocity(DeltaTime);

}

void AMyKartPawn::UpdateRotationFromFQuat(float DeltaTime)
{
	float RotationAngle = MaxDegreePerSecond * DeltaTime * SteeringThrow; // {(degree/s) * 초 = 이 프레임에서 회전 할 도수} * SteeringThrow  ** 주의 : 도수임. Radian아님
	FQuat RotationDelta(GetActorUpVector(), FMath::DegreesToRadians(RotationAngle)); // DeltaTime에 따라 특정 각도로 회전하는데 사용
	Velocity = RotationDelta.RotateVector(Velocity); // Velocity의 방향을 똑같은 양만큼 회전시켜준다.
	AddActorWorldRotation(RotationDelta, true);
	/*
	GetActorUpVector() : 우리의 차를 찌를 펜. 즉, 축이 될 것이다. 축을 기준으로 회전
	RotationAngle : 프레임에서 회전 할 도수와 방향이다.
	*/
}

void AMyKartPawn::UpdateLocationFromVelocity(float DeltaTime)
{
	// Translation은 cm 단위이다. 적절한 단위 변환 필요
	FVector Translation = 100 * Velocity * DeltaTime; // 속도(m/s)에 초단위 시간을 곱하면 m가 나온다. 여기에 cm로 변환하기 위해 100을 곱한다.


	FHitResult HitResult;
	AddActorWorldOffset(Translation, true, &HitResult); // 월드 공간에서 이 액터의 위치에 델타를 추가합니다.
	if (HitResult.IsValidBlockingHit()) {
		// 특정 이동 프레임에서 실제로 무언가를 쳤으면 true 반환
		Velocity = FVector::ZeroVector; // 이 설정을 안해주면, 후진 할 때 기존의 Velocity에서 다시 빼서 음수가 될때까지 후진을 하지 않는다.
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
	// Velocity는 방향과 속도, 전진 or 후진이 주어져야 한다.
	//Velocity = GetActorForwardVector() * 20 * Value;  // 20m/s

	// 추진력
	Throttle = Value;
}

void AMyKartPawn::MoveRight(float Value)
{
	SteeringThrow = Value;
}
