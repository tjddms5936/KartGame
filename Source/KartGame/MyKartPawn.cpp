// Fill out your copyright notice in the Description page of Project Settings.


#include "MyKartPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/InputComponent.h"
#include "Net/UnrealNetWork.h"
// Sets default values
AMyKartPawn::AMyKartPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AMyKartPawn::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) {
		NetUpdateFrequency = 1; // 디폴트 값은 더 작을 것이다.
	}
}

void AMyKartPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyKartPawn, ServerState);
	
}

// Called every frame
void AMyKartPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled()) {
		// 이 클라이언트에 컨트롤러가 있는지 여부를 확인하는 함수. 즉, 클라이언트라면 아래 작업 수행
		// 즉 클라이언트가 Move를 생성해서 서버로 보내는 작업이다.
		FMyKartMove Move = CreateMove(DeltaTime);  // OnTick Step1

		// 서버인 경우에는 큐에 추가 할 필요가 없음
		if (!HasAuthority()) {
			// 클라이언트인 경우에만
			UnAcknowlegedmoves.Add(Move); // OnTick Step2
			UE_LOG(LogTemp, Warning, TEXT("UnAcknowlegedmoves Length : %d"), UnAcknowlegedmoves.Num());
		}

		Server_SendMove(Move); // OnTick Step3
		SimulateMove(Move); // // OnTick Step4 스스로 시뮬레이션 호출
	}

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(GetLocalRole()), this, FColor::White, DeltaTime); 
	DrawDebugString(GetWorld(), FVector(0, 0, -200), FString::Printf(TEXT(" %d km/h"), (int)Velocity.Size()), this, FColor::White, DeltaTime);

}

void AMyKartPawn::UpdateRotationFromFQuat(float DeltaTime, float SteeringThrow_)
{

	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity ) * DeltaTime; // 방향까지 포함된 속도 구하기
	float RotationAngle = DeltaLocation / MinTurningRate * SteeringThrow_; // {(degree/s) * 초 = 이 프레임에서 회전 할 도수} * SteeringThrow  ** 주의 : 도수임. Radian아님
	FQuat RotationDelta(GetActorUpVector(), RotationAngle); // DeltaTime에 따라 특정 각도로 회전하는데 사용
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

FVector AMyKartPawn::GetAirResistance()
{
	// AirResistance = -FMath::Pow(Velocity.Size(), 2) * DragCoefficient;
	// Velocity.GetSafeNormal()은 방향이다.
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector AMyKartPawn::GetRollingResistance()
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

void AMyKartPawn::SimulateMove(FMyKartMove Move)
{
	// 이제 입력에 대한 모든 데이터는 액터에서 직접 오는게 아니라 Move를 통해 들어온다 
	FVector Force = GetActorForwardVector() * MaxDrivingForce * Move.Throttle; // 힘 = 방향 * 최대추진력 * 조절력
	Force += GetAirResistance(); // 저항력을 추진력에 추가
	Force += GetRollingResistance(); // 구르기 저항을 추진력에 추가

	FVector Acceleration = Force / Mass; // a 구하기

	Velocity = Velocity + Acceleration * Move.DeltaTime; // 속도를 얻기 위해서는 기존 속도 + 속도의 변화(dt)

	UpdateRotationFromFQuat(Move.DeltaTime, Move.SteeringThrow);

	UpdateLocationFromVelocity(Move.DeltaTime);
}

FMyKartMove AMyKartPawn::CreateMove(float DeltaTime)
{
	FMyKartMove move;
	move.DeltaTime = DeltaTime;
	move.Throttle = Throttle;
	move.SteeringThrow = SteeringThrow;
	move.Time = GetWorld()->TimeSeconds; // 게임 시뮬레이션 시간. 타임 스탬프로써 활용 할 것이다. 
	return move;
}

void AMyKartPawn::ClearUnAcknowlegemoves(FMyKartMove LastMove)
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

void AMyKartPawn::OnRep_ServerState()
{
	// 자율 프록시(﻿AutonomousProxy) 또는 시뮬레이션된 프록시(SimulatedProxy) 의미. 따라서 서버로부터 복제된 위치를 설정하려고 함
	// 이 부분이 바로 수도코드의 OnReceiveServerState의	Step2 과정이다. (Server State로 Reset)
	SetActorTransform(ServerState.Transform);
	Velocity = ServerState.Velocity;

	ClearUnAcknowlegemoves(ServerState.LastMove); // 큐 정리
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
	Throttle = Value;
}

void AMyKartPawn::MoveRight(float Value)
{
	SteeringThrow = Value;
}

void AMyKartPawn::Server_SendMove_Implementation(FMyKartMove Move)
{
	SimulateMove(Move);
	ServerState.LastMove = Move; // 방금 시뮬레이션 한 Move가 됨
	// Tick에서 작업하던게 여기로 옮겨짐. 이 작업을 통해 더이상 Throttle, SteeringThrow는 Replicated 될 필요가 없음. 
	// 아래 작업을 통해 완벽하게 복제됨.
	ServerState.Transform = GetActorTransform(); // 클라이언트에게 표준 상태를 보내는 코드
	ServerState.Velocity = Velocity; // 클라이언트에게 표준 Velocity 보내기
}

bool AMyKartPawn::Server_SendMove_Validate(FMyKartMove Move)
{
	return true; // TODO
}
