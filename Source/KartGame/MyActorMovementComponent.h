// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyActorMovementComponent.generated.h"


USTRUCT()
struct FMyKartMove
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY() // 개별적으로 복제할 필요는 없으므로 Replicated는 필요없다.
	float Throttle; // 조절판. Throttle으로부터 Force -> Acceleration -> Velocity -> Translation  순으로 구해 나간다.

	UPROPERTY()
	float SteeringThrow; // 회전 방향

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float Time; // 이동이 시작된 시간
	// 이전의 이동을 제거할 수 있도록 
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KARTGAME_API UMyActorMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyActorMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SimulateMove(const FMyKartMove& Move);

	FMyKartMove CreateMove(float DeltaTime);

	void UpdateRotationFromFQuat(float DeltaTime, float SteeringThrow_);

	void UpdateLocationFromVelocity(float DeltaTime);

	FVector GetAirResistance();

	FVector GetRollingResistance();

	void SetThrottle(float Value) { Throttle = Value; };

	void SetSteeringThrow(float Value) { SteeringThrow = Value; };

	void SetVelocity(FVector Value) { Velocity = Value; };

	FVector GetVelocity() { return Velocity; };

	FMyKartMove GetLastMove() { return LastMove; };

private:
	FMyKartMove LastMove;

	FVector Velocity;

	// 자동차의 질량 [단위 : kg]
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float Mass = 1000;

	// 조절판을 최대로 내렸을때 차에 가해지는 힘 [단위 : N (뉴턴)]
	// 가속도의 단위는 m/(s*s) 즉, 미터 퍼 섹크 제곱
	// 1000kg은 1톤인데, 이를 위해서라면 10000뉴턴이 필요한다. 10의 가속도를 얻을 수 있기 떄문이다.
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float MaxDrivingForce = 10000;

	// 완전히 돌린 상태에서 자동차 회전 반경의 최소 반지름 [단위 : m]
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float MinTurningRate = 10; // 초당 회전 반경. 대부분의 차의 경우 약 10미터 이다.

		// 높을수록 더 많은 항력 [단위 : kg/m]
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float DragCoefficient = 16; // = MaxDrivingForce / Speed^2 = 10000 / 25^2 = 16

		// 높을수록 더 많은 항력 [단위 : kg/m]
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float RRCoefficient = 0.015f; // = 타이어 종류에 따라 다양한 계수들 존재. 위키피디아 참조


	// 조절판
	float Throttle; // Throttle으로부터 Force -> Acceleration -> Velocity -> Translation  순으로 구해 나간다.
	float SteeringThrow; // 회전 방향
		
};
