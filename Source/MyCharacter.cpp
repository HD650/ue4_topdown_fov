// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//trival intial code
	this->static_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	this->static_mesh->SetupAttachment((USceneComponent*)this->GetCapsuleComponent());
	//Procedural generated mesh
	this->fov_mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("FOV_MESH"));
	this->fov_mesh->SetupAttachment((USceneComponent*)this->GetCapsuleComponent());
	//use this mesh as stencil, so do not render to the scene
	this->fov_mesh->bRenderCustomDepth = true;
	this->fov_mesh->CustomDepthStencilValue = 255;
	this->fov_mesh->bRenderInMainPass = false;
	this->fov_mesh->bUseAsyncCooking = true;

	//calculate fov
	this->rotation_each_step = fov / steps;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* contorller = this->GetWorld()->GetFirstPlayerController();
	contorller->bShowMouseCursor = true;

	//calculate fov
	this->rotation_each_step = fov / steps;
}

void AMyCharacter::FaceMouse()
{
	//perspective camera runs wrong here because the DeprojectMousePositionToWorld function
	//get the world location of mouse in the near plane of camera which is very far from the ground
	//to fix this, need to cast a ray to the gorund and get the hit result as the final location
	APlayerController* contorller = this->GetWorld()->GetFirstPlayerController();
	FVector mouse_location, mouse_rotation;
	contorller->DeprojectMousePositionToWorld(mouse_location, mouse_rotation);
	//UE_LOG(LogTemp, Warning, TEXT("Location %f %f %f"), mouse_location.X, mouse_location.Y, mouse_location.Z);
	FRotator currentCharacterRotation = this->GetActorRotation();
	FRotator targetRotation = mouse_rotation.Rotation();

	FVector cur_location = this->GetActorLocation();
	//UE_LOG(LogTemp, Warning, TEXT("Location %f %f %f"), cur_location.X, cur_location.Y, cur_location.Z);
	mouse_location.Z = cur_location.Z;
	cur_location = mouse_location - cur_location;
	cur_location.Normalize();

	//UE_LOG(LogTemp, Warning, TEXT("Normal %f %f %f"), cur_location.X, cur_location.Y, cur_location.Z);
	FRotator newRotation = FRotator(currentCharacterRotation.Pitch, targetRotation.Yaw, currentCharacterRotation.Roll);
	this->SetActorRotation(cur_location.Rotation());
}

void AMyCharacter::LineTrace()
{
	//calculate fov
	this->rotation_each_step = fov / steps;

	FRotator cur_rot = this->GetActorRotation();
	FRotator step_rotator = FRotator(cur_rot.Pitch, cur_rot.Yaw-(fov/2), cur_rot.Roll);
	FVector cur_location = this->GetActorLocation();
	//UE_LOG(LogTemp, Warning, TEXT("rotation P%f Y%f R%f"), cur_rot.Pitch, cur_rot.Yaw, cur_rot.Roll);

	//data to procedurally generate mesh
	TArray<FVector> vertices;
	vertices.Add(FVector(0,0,0));

	//ray casting to calculate field of view
	for (int i = 0; i < this->steps; i++)
	{
		FVector direction = step_rotator.Vector();
		FHitResult hit;
		bool is_hit = GetWorld()->LineTraceSingleByChannel(hit, cur_location, cur_location + (sight_distance*direction), ECollisionChannel::ECC_Visibility);
		if (is_hit)
		{
			//DrawDebugLine(this->GetWorld(), cur_location, hit.Location, FColor::Blue);
			FVector hitpoint_local = this->GetTransform().InverseTransformPosition(hit.Location);
			vertices.Add(hitpoint_local);
		}
		else
		{
			//DrawDebugLine(this->GetWorld(), cur_location, cur_location + (sight_distance*direction), FColor::Red);
			FVector sightpoint_local = this->GetTransform().InverseTransformPosition(cur_location+(sight_distance*direction));
			vertices.Add(sightpoint_local);
		}
		step_rotator.Yaw += this->rotation_each_step;
	}

	//use inversed index since only one side of mesh will be rendered
	// index 012 will face down, index 021 will face up
	TArray<int32> index;
	int triangle_num = this->steps - 1;
	for (int i = 0; i < triangle_num; i++)
	{
		index.Add(0);
		index.Add(i+2);
		index.Add(i+1);
	}

	//generate mesh
	this->fov_mesh->CreateMeshSection_LinearColor(0, vertices, index, TArray<FVector>(), TArray<FVector2D>(), TArray<FLinearColor>(), TArray<FProcMeshTangent>(), false);
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FaceMouse();
	LineTrace();
}

void AMyCharacter::MoveHorizontal(float scale)
{
	FRotator Rotation = this->Controller->GetControlRotation();
	// add movement in that direction
	const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, scale);
}

void AMyCharacter::MoveVertical(float scale)
{
	FRotator Rotation = this->Controller->GetControlRotation();
	// add movement in that direction
	const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
	this->AddMovementInput(Direction, scale);
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Horizontal", this, &AMyCharacter::MoveHorizontal);
	PlayerInputComponent->BindAxis("Vertical", this, &AMyCharacter::MoveVertical);
}

