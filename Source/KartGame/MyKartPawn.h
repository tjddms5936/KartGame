// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyKartPawn.generated.h"

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

USTRUCT()
struct FMyKartState
{
	GENERATED_USTRUCT_BODY()
public:
	// 보간을 하려면 Throttle이 필요하다. 복제된 프로퍼티 중 하나이다. 
	// 다른 복제된 프로퍼티는 무엇이 있을까? Velocity
	UPROPERTY()
	FVector Velocity; // 우리가 시뮬레이션 할 때 상태의 매우 중요한 부분이다. 

	UPROPERTY()
	FTransform Transform; // 마지막 이동 
	/* 
	자율 프록시﻿(AutonomousProxy)가 아닌 경우 즉, 서버. ﻿(로컬 상태를 수신하는 서버의 OnReceiveMove )﻿
	속도와 트랜스폼이 있으므로 상태를 클라이언트에 보낼 수 있고, 클라이언트는 우리가 그걸 보냈기 떄문에 시뮬레이션 할 수 있다.
	마지막 이동도 적용되어 그 사이에 계속 보간할 수 있다.
	*/

	FMyKartMove LastMove; // 이 상태를 만드는 마지막 이동이 될 것이며, 우리가 비자율인 시뮬레이션된 프록시에 있기 떄문에 도움이 될 것이다. 
	/*
	자율 프록시﻿(AutonomousProxy)인 경우 즉, 클라이언트.(서버 상태를 수신하는 로컬﻿의  ﻿OnReceiveServerState)
	Step1. ﻿상태에 포함된 모든 이동을 제거. How? 
	우리는 LastMove 즉 마지막 이동을 가지고 있다. 이것과 비교하며 확인할 수 있다. 그러나 DeltaTime이 정확히 동일한 두가지 이동을 연속적으로
	연달아 실행한다면 우리는 이것을 구별할 방법이 필요하다. 원한다면 그것들에 일렬번호를 추가할 수도 있지만
	여기서의 방법은 단순히 타임스탬프(float Time)을 추가하는 것이다. 즉 이동이 시작된 시간을 갖는 것이다. 
	이렇게 하면 서버에서 마지막 이동(LastMove)을 수신할 때 승인되지 않은 이동 목록을 살펴보고
	해당 이동이 해당 마지막 이동 이전인지, 혹은 같은지 확인할 수 있다. 그리고 그 경우 우리는 오래된 이동을 제거할 수 있다. 
	결국 새로운 이동으로 계속 교체되어 유지된다.
 	*/
};

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

	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	UFUNCTION(Server, Reliable, WithValidation) // ServerRPCFunction이라는 의미
	void Server_SendMove(FMyKartMove Move); // 이제 Server_SendMove라는 하나의 RPC만 있다.
	/*
	// 앞으로 가는 조작
	UFUNCTION(Server, Reliable, WithValidation) // ServerRPCFunction이라는 의미
	void Server_MoveForward(float Value);

	// 회전하는 조작
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);
	*/

	UFUNCTION() // 이 함수는 기본적으로 복제 알림이 되는 이벤트로 호출되기 위해 UFUNCTION이어야 한다.
	void OnRep_ServerState();

	// OnRep_ReplicatedLocation, OnRep_ReplicatedRotation 삭제 후 OnRep_ServerState로 대체
	
	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FMyKartState ServerState;

	UPROPERTY()
	FMyKartMove ServerMove;
	
private:
	// Functions
	void UpdateRotationFromFQuat(float DeltaTime);
	void UpdateLocationFromVelocity(float DeltaTime);
	FVector GetAirResistance();
	FVector GetRollingResistance();

	FString GetEnumText(ENetRole Role);

private:
	// Variables

	UPROPERTY(replicated)
	FVector Velocity;

	// 조절판
	UPROPERTY(replicated)
	float Throttle; // Throttle으로부터 Force -> Acceleration -> Velocity -> Translation  순으로 구해 나간다.

	UPROPERTY(replicated)
	float SteeringThrow; // 회전 방향

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



};
