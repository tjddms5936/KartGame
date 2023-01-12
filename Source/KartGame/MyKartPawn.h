// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyActorMovementComponent.h"
#include "MyKartMovementReplicator.h"
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

	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	

private:
	// Functions
	
	FString GetEnumText(ENetRole Role);

private:
	// Variables

	UPROPERTY(VisibleAnywhere)
	UMyActorMovementComponent* MyMovementComponent;

	UPROPERTY(VisibleAnywhere)
	UMyKartMovementReplicator* MyKartMovementReplicator;
};
