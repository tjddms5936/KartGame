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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom Setting")
	class USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Setting")
	class USkeletalMeshComponent* MySkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Setting")
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Setting")
	class USpringArmComponent* SpringArm;

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

private:
	FVector Velocity; // �ӵ� : �ӷ�(speed)�� ������ ������ ��

};
