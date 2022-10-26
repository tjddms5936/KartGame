// Fill out your copyright notice in the Description page of Project Settings.


#include "MyKartPawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"

// Sets default values
AMyKartPawn::AMyKartPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	MySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MySkeletalMesh"));
	MySkeletalMesh->SetupAttachment(GetRootComponent());

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(MySkeletalMesh);
	SpringArm->TargetArmLength = 450.f; // 기본 거리
	SpringArm->bUsePawnControlRotation = false; // 컨트롤러에 맞춰서 회전

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // 스프링 암에서 true로 설정해주었기 떄문?
	
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

	// Translation은 cm 단위이다. 적절한 단위 변환 필요
	FVector Translation = 100 * Velocity * DeltaTime; // 속도(m/s)에 초단위 시간을 곱하면 m가 나온다. 여기에 cm로 변환하기 위해 100을 곱한다.
	AddActorWorldOffset(Translation); // 월드 공간에서 이 액터의 위치에 델타를 추가합니다.
}

// Called to bind functionality to input
void AMyKartPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyKartPawn::MoveForward);

}

void AMyKartPawn::MoveForward(float Value)
{
	// Velocity는 방향과 속도, 전진 or 후진이 주어져야 한다.
	Velocity = GetActorForwardVector() * 20 * Value;  // 20m/s
}

