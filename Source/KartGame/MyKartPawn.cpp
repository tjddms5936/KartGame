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
	SpringArm->TargetArmLength = 450.f; // �⺻ �Ÿ�
	SpringArm->bUsePawnControlRotation = false; // ��Ʈ�ѷ��� ���缭 ȸ��

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // ������ �Ͽ��� true�� �������־��� ����?
	
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

	// Translation�� cm �����̴�. ������ ���� ��ȯ �ʿ�
	FVector Translation = 100 * Velocity * DeltaTime; // �ӵ�(m/s)�� �ʴ��� �ð��� ���ϸ� m�� ���´�. ���⿡ cm�� ��ȯ�ϱ� ���� 100�� ���Ѵ�.
	AddActorWorldOffset(Translation); // ���� �������� �� ������ ��ġ�� ��Ÿ�� �߰��մϴ�.
}

// Called to bind functionality to input
void AMyKartPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyKartPawn::MoveForward);

}

void AMyKartPawn::MoveForward(float Value)
{
	// Velocity�� ����� �ӵ�, ���� or ������ �־����� �Ѵ�.
	Velocity = GetActorForwardVector() * 20 * Value;  // 20m/s
}

