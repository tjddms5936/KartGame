// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyActorMovementComponent.h"
#include "MyKartMovementReplicator.generated.h"

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
	UPROPERTY()
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


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KARTGAME_API UMyKartMovementReplicator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyKartMovementReplicator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
private:
	UFUNCTION(Server, Reliable, WithValidation) // ServerRPCFunction이라는 의미
	void Server_SendMove(FMyKartMove Move); // 이제 Server_SendMove라는 하나의 RPC만 있다.

	UFUNCTION() // 이 함수는 기본적으로 복제 알림이 되는 이벤트로 호출되기 위해 UFUNCTION이어야 한다.
	void OnRep_ServerState();

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FMyKartState ServerState;

	TArray<FMyKartMove> UnAcknowlegedmoves; // 클라이언트에만 저장되는 데이터

	void ClearUnAcknowlegemoves(FMyKartMove LastMove);

	UPROPERTY()
	UMyActorMovementComponent* MovementComponent;

	void UpdateServerState(const FMyKartMove& LastMove);
};
