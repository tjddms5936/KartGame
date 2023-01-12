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
	UPROPERTY() // ���������� ������ �ʿ�� �����Ƿ� Replicated�� �ʿ����.
	float Throttle; // ������. Throttle���κ��� Force -> Acceleration -> Velocity -> Translation  ������ ���� ������.

	UPROPERTY()
	float SteeringThrow; // ȸ�� ����

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float Time; // �̵��� ���۵� �ð�
	// ������ �̵��� ������ �� �ֵ��� 
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

	// �ڵ����� ���� [���� : kg]
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float Mass = 1000;

	// �������� �ִ�� �������� ���� �������� �� [���� : N (����)]
	// ���ӵ��� ������ m/(s*s) ��, ���� �� ��ũ ����
	// 1000kg�� 1���ε�, �̸� ���ؼ���� 10000������ �ʿ��Ѵ�. 10�� ���ӵ��� ���� �� �ֱ� �����̴�.
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float MaxDrivingForce = 10000;

	// ������ ���� ���¿��� �ڵ��� ȸ�� �ݰ��� �ּ� ������ [���� : m]
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float MinTurningRate = 10; // �ʴ� ȸ�� �ݰ�. ��κ��� ���� ��� �� 10���� �̴�.

		// �������� �� ���� �׷� [���� : kg/m]
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float DragCoefficient = 16; // = MaxDrivingForce / Speed^2 = 10000 / 25^2 = 16

		// �������� �� ���� �׷� [���� : kg/m]
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float RRCoefficient = 0.015f; // = Ÿ�̾� ������ ���� �پ��� ����� ����. ��Ű�ǵ�� ����


	// ������
	float Throttle; // Throttle���κ��� Force -> Acceleration -> Velocity -> Translation  ������ ���� ������.
	float SteeringThrow; // ȸ�� ����
		
};
