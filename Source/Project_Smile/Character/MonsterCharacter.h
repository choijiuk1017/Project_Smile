// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterCharacter.generated.h"

class UBehaviorTree;
class USphereComponent;


UCLASS()
class PROJECT_SMILE_API AMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonsterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	bool bHasDetectedPlayer;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	bool bIsLookingUp;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	bool bCanChase;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	AActor* TargetActor;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* DetectSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jumpscare")
	USphereComponent* JumpscareCollision;

	UFUNCTION()
	void OnDetectSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnJumpscareCollisionBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jumpscare")
	class UCameraComponent* JumpscareCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jumpscare")
	class UAnimMontage* JumpscareMontage;

	UPROPERTY(BlueprintReadOnly, Category = "Jumpscare")
	bool bIsJumpscaring = false;

	UFUNCTION(BlueprintCallable, Category = "Jumpscare")
	void StartJumpscare(AActor* PlayerActor);

	UFUNCTION()
	void OnJumpscareMontageEnded(UAnimMontage* Montage, bool bInterrupted);


public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	void FinishLookUp();

private:
	void UpdateBlackboard();

	bool IsPlayerOnNavMesh(AActor* PlayerActor) const;
};
