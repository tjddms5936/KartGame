// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActorMovementComponent.h"

// Sets default values for this component's properties
UMyActorMovementComponent::UMyActorMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMyActorMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	
	
}


// Called every frame
void UMyActorMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (GetOwnerRole() == ROLE_AutonomousProxy || GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy) {
		LastMove = CreateMove(DeltaTime);  // OnTick Step1
		SimulateMove(LastMove);
	}
}

void UMyActorMovementComponent::UpdateRotationFromFQuat(float DeltaTime, float SteeringThrow_)
{

	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime; // 방향까지 포함된 속도 구하기
	float RotationAngle = DeltaLocation / MinTurningRate * SteeringThrow_; // {(degree/s) * 초 = 이 프레임에서 회전 할 도수} * SteeringThrow  ** 주의 : 도수임. Radian아님
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle); // DeltaTime에 따라 특정 각도로 회전하는데 사용
	Velocity = RotationDelta.RotateVector(Velocity); // Velocity의 방향을 똑같은 양만큼 회전시켜준다.
	GetOwner()->AddActorWorldRotation(RotationDelta, true);
	/*
	GetActorUpVector() : 우리의 차를 찌를 펜. 즉, 축이 될 것이다. 축을 기준으로 회전
	RotationAngle : 프레임에서 회전 할 도수와 방향이다.
	*/
}

void UMyActorMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	// Translation은 cm 단위이다. 적절한 단위 변환 필요
	FVector Translation = 100 * Velocity * DeltaTime; // 속도(m/s)에 초단위 시간을 곱하면 m가 나온다. 여기에 cm로 변환하기 위해 100을 곱한다.


	FHitResult HitResult;
	GetOwner()->AddActorWorldOffset(Translation, true, &HitResult); // 월드 공간에서 이 액터의 위치에 델타를 추가합니다.
	if (HitResult.IsValidBlockingHit()) {
		// 특정 이동 프레임에서 실제로 무언가를 쳤으면 true 반환
		Velocity = FVector::ZeroVector; // 이 설정을 안해주면, 후진 할 때 기존의 Velocity에서 다시 빼서 음수가 될때까지 후진을 하지 않는다.
	}
}

FVector UMyActorMovementComponent::GetAirResistance()
{
	// AirResistance = -FMath::Pow(Velocity.Size(), 2) * DragCoefficient;
	// Velocity.GetSafeNormal()은 방향이다.
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector UMyActorMovementComponent::GetRollingResistance()
{
	/*
	﻿RollingResistance = RRCoefficient x NormalForce﻿에서
	NormalForce﻿에서 = m * g 인데, 언리얼에서 기본적으로 g가 음수이므로 음수 변환으로 반환해준다.
	방향은 여전히 SafeNormal이 될 것이다. 왜냐면 속도에 반대로 갈 것이기 때문이다.
	구르기 저항은 여전히 저항이므로 운동 방향의 반대로 작용한다는 것 알아두기
	*/
	float AccelerationDueToGravity = -(GetWorld()->GetGravityZ()) / 100;
	float ﻿NormalForce = Mass * AccelerationDueToGravity;
	return -Velocity.GetSafeNormal() * RRCoefficient * ﻿NormalForce;
}

void UMyActorMovementComponent::SimulateMove(const FMyKartMove& Move)
{
	// 이제 입력에 대한 모든 데이터는 액터에서 직접 오는게 아니라 Move를 통해 들어온다 
	FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle; // 힘 = 방향 * 최대추진력 * 조절력
	Force += GetAirResistance(); // 저항력을 추진력에 추가
	Force += GetRollingResistance(); // 구르기 저항을 추진력에 추가

	FVector Acceleration = Force / Mass; // a 구하기

	Velocity = Velocity + Acceleration * Move.DeltaTime; // 속도를 얻기 위해서는 기존 속도 + 속도의 변화(dt)

	UpdateRotationFromFQuat(Move.DeltaTime, Move.SteeringThrow);

	UpdateLocationFromVelocity(Move.DeltaTime);
}

FMyKartMove UMyActorMovementComponent::CreateMove(float DeltaTime)
{
	FMyKartMove move;
	move.DeltaTime = DeltaTime;
	move.Throttle = Throttle;
	move.SteeringThrow = SteeringThrow;
	move.Time = GetWorld()->TimeSeconds; // 게임 시뮬레이션 시간. 타임 스탬프로써 활용 할 것이다. 
	return move;
}