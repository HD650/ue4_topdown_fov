// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "ProceduralMeshComponent.h"
#include "MyCharacter.generated.h"

UCLASS()
class UE2D_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveHorizontal(float scale);
	void MoveVertical(float scale);
	void FaceMouse();
	void LineTrace();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* static_mesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float fov;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int steps;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float sight_distance;
	float rotation_each_step;
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* fov_mesh;
};
