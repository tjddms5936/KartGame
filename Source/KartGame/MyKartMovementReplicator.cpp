// Fill out your copyright notice in the Description page of Project Settings.


#include "MyKartMovementReplicator.h"
#include "Net/UnrealNetWork.h"

// Sets default values for this component's properties
UMyKartMovementReplicator::UMyKartMovementReplicator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	// ...
}


// Called when the game starts
void UMyKartMovementReplicator::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent = GetOwner()->FindComponentByClass<UMyActorMovementComponent>();
	
}


// Called every frame
void UMyKartMovementReplicator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (MovementComponent == nullptr) return;

	FMyKartMove LastMove = MovementComponent->GetLastMove();

	if (GetOwnerRole() == ROLE_AutonomousProxy) {
		// 클라이언트라면
		UnAcknowlegedmoves.Add(LastMove); // OnTick Step2
		Server_SendMove(LastMove); // OnTick Step3
	}
	if (GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy) {
		// 위의 조건이 곧 서버라는 의미
		// 서버이면서 서버가 컨트롤하고있는 화면이라면
		UpdateServerState(LastMove);
	}
	if (GetOwnerRole() == ROLE_SimulatedProxy) {
		MovementComponent->SimulateMove(ServerState.LastMove); // // OnTick Step4 스스로 시뮬레이션 호출
	}
}


void UMyKartMovementReplicator::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMyKartMovementReplicator, ServerState);

}

void UMyKartMovementReplicator::ClearUnAcknowlegemoves(FMyKartMove LastMove)
{
	// LastMove는 NewMoves 배열을 반복한다.
	TArray<FMyKartMove> NewMoves; // LastMvoe 시간보다 더 큰 것들

	if (UnAcknowlegedmoves.Num() > 0) {
		// 참조가 될 것이므로 항상 복사 할 필요가 없어 const를 쓴다.즉, 수정하지 않을 것이라서
		for (const FMyKartMove& Move : UnAcknowlegedmoves) {
			// 마지막 Move시간(ServerMove.Time) 보다 작거나 같은 모든 항목은 이제 의미가 없어짐.
			if (Move.Time > LastMove.Time) {
				NewMoves.Add(Move);
			}
		}
		UnAcknowlegedmoves = NewMoves;
	}

	// 이 함수는 언제 호출 되어야 할까? : 클라이언트가 서버에서 NewState를 '수신'할 때인 OnRep_ServerState 이다. 
}

void UMyKartMovementReplicator::UpdateServerState(const FMyKartMove& LastMove)
{
	ServerState.LastMove = LastMove; // 방금 시뮬레이션 한 Move가 됨
	// Tick에서 작업하던게 여기로 옮겨짐. 이 작업을 통해 더이상 Throttle, SteeringThrow는 Replicated 될 필요가 없음. 
	// 아래 작업을 통해 완벽하게 복제됨.
	ServerState.Transform = GetOwner()->GetActorTransform(); // 클라이언트에게 표준 상태를 보내는 코드
	ServerState.Velocity = MovementComponent->GetVelocity(); // 클라이언트에게 표준 Velocity 보내기
}

void UMyKartMovementReplicator::OnRep_ServerState()
{
	// 자율 프록시(﻿AutonomousProxy) 또는 시뮬레이션된 프록시(SimulatedProxy) 의미. 따라서 서버로부터 복제된 위치를 설정하려고 함
	// 이 부분이 바로 수도코드의 OnReceiveServerState의	Step2 과정이다. (Server State로 Reset)
	if (MovementComponent == nullptr) return;
	GetOwner()->SetActorTransform(ServerState.Transform);
	MovementComponent->SetVelocity(ServerState.Velocity);

	ClearUnAcknowlegemoves(ServerState.LastMove); // 큐 정리

	for (const FMyKartMove& Move : UnAcknowlegedmoves) {
		MovementComponent->SimulateMove(Move);
	}
}

void UMyKartMovementReplicator::Server_SendMove_Implementation(FMyKartMove Move)
{
	if (MovementComponent == nullptr) return;
	MovementComponent->SimulateMove(Move);
	UpdateServerState(Move);
}

bool UMyKartMovementReplicator::Server_SendMove_Validate(FMyKartMove Move)
{
	return true; // TODO
}