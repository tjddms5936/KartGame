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

	// ���� �ִ� �������� �ǵ��� ����
	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle; // �� = ���� * �ִ������� * ������
	FVector Acceleration = Force / Mass; // a ���ϱ�
	Velocity = Velocity + Acceleration * DeltaTime; // �ӵ��� ��� ���ؼ��� ���� �ӵ� + �ӵ��� ��ȭ(dt)

	UpdateRotationFromFQuat(DeltaTime);
	

	UpdateLocationFromVelocity(DeltaTime);

}

void AMyKartPawn::UpdateRotationFromFQuat(float DeltaTime)
{
	float RotationAngle = MaxDegreePerSecond * DeltaTime * SteeringThrow; // {(degree/s) * �� = �� �����ӿ��� ȸ�� �� ����} * SteeringThrow  ** ���� : ������. Radian�ƴ�
	FQuat RotationDelta(GetActorUpVector(), FMath::DegreesToRadians(RotationAngle)); // DeltaTime�� ���� Ư�� ������ ȸ���ϴµ� ���
	Velocity = RotationDelta.RotateVector(Velocity); // Velocity�� ������ �Ȱ��� �縸ŭ ȸ�������ش�.
	AddActorWorldRotation(RotationDelta, true);
	/*
	GetActorUpVector() : �츮�� ���� � ��. ��, ���� �� ���̴�. ���� �������� ȸ��
	RotationAngle : �����ӿ��� ȸ�� �� ������ �����̴�.
	*/
}

void AMyKartPawn::UpdateLocationFromVelocity(float DeltaTime)
{
	// Translation�� cm �����̴�. ������ ���� ��ȯ �ʿ�
	FVector Translation = 100 * Velocity * DeltaTime; // �ӵ�(m/s)�� �ʴ��� �ð��� ���ϸ� m�� ���´�. ���⿡ cm�� ��ȯ�ϱ� ���� 100�� ���Ѵ�.


	FHitResult HitResult;
	AddActorWorldOffset(Translation, true, &HitResult); // ���� �������� �� ������ ��ġ�� ��Ÿ�� �߰��մϴ�.
	if (HitResult.IsValidBlockingHit()) {
		// Ư�� �̵� �����ӿ��� ������ ���𰡸� ������ true ��ȯ
		Velocity = FVector::ZeroVector; // �� ������ �����ָ�, ���� �� �� ������ Velocity���� �ٽ� ���� ������ �ɶ����� ������ ���� �ʴ´�.
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
	// Velocity�� ����� �ӵ�, ���� or ������ �־����� �Ѵ�.
	//Velocity = GetActorForwardVector() * 20 * Value;  // 20m/s

	// ������
	Throttle = Value;
}

void AMyKartPawn::MoveRight(float Value)
{
	SteeringThrow = Value;
}
