// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyKartPawn.generated.h"

UCLASS()
class KARTGAME_API AMyKartPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyKartPawn();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ������ ���� ����
	void MoveForward(float Value);

	// ȸ���ϴ� ����
	void MoveRight(float Value);

	
private:
	// Functions
	void UpdateRotationFromFQuat(float DeltaTime);
	void UpdateLocationFromVelocity(float DeltaTime);
	FVector GetAirResistance();


private:
	// Variables
	FVector Velocity; // �ӵ� : �ӷ�(speed)�� ������ ������ ��

	// �ڵ����� ���� [���� : kg]
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float Mass = 1000;

	// �������� �ִ�� �������� ���� �������� �� [���� : N (����)]
	// ���ӵ��� ������ m/(s*s) ��, ���� �� ��ũ ����
	// 1000kg�� 1���ε�, �̸� ���ؼ���� 10000������ �ʿ��Ѵ�. 10�� ���ӵ��� ���� �� �ֱ� �����̴�.
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float MaxDrivingForce = 10000;

	// �ʴ� ȸ���� �� �ִ� �ִ� ����[���� : degree/s]
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float MaxDegreePerSecond = 90;

	// �������� �� ���� �׷� [���� : kg/m]
	UPROPERTY(EditAnywhere, Category = "Custom Setting")
	float DragCoefficient = 16; // = MaxDrivingForce / Speed^2 = 10000 / 25^2 = 16

	// ������
	float Throttle; // Throttle���κ��� Force -> Acceleration -> Velocity -> Translation  ������ ���� ������.
	float SteeringThrow; // ȸ�� ����
};
