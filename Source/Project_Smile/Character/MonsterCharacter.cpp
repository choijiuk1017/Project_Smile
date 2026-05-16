// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MonsterCharacter.h"
#include "Character/MonsterAIController.h"
#include "Project_SmileCharacter.h"


#include "Components/SphereComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Animation/AnimInstance.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "NavigationSystem.h"
#include "Engine/TargetPoint.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"


AMonsterCharacter::AMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AMonsterAIController::StaticClass();

	DetectSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectSphere"));
	DetectSphere->SetupAttachment(RootComponent);

	DetectSphere->SetSphereRadius(600.0f);
	DetectSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectSphere->SetCollisionObjectType(ECC_WorldDynamic);
	DetectSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	JumpscareCollision = CreateDefaultSubobject<USphereComponent>(TEXT("JumpscareCollision"));
	JumpscareCollision->SetupAttachment(RootComponent);

	JumpscareCollision->SetSphereRadius(120.0f);
	JumpscareCollision->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));

	JumpscareCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	JumpscareCollision->SetCollisionObjectType(ECC_WorldDynamic);
	JumpscareCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	JumpscareCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	JumpscareCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("JumpscareCamera"));
	JumpscareCamera->SetupAttachment(RootComponent);
	JumpscareCamera->bAutoActivate = false;

	bHasDetectedPlayer = false;
	bIsIdle = false;
	bIsLookingUp = false;
	bCanChase = false;
	bIsEscaping = false;
	TargetActor = nullptr;
}

void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	bIsIdle = true;

	if (DetectSphere)
	{
		DetectSphere->OnComponentBeginOverlap.AddDynamic(
			this,
			&AMonsterCharacter::OnDetectSphereBeginOverlap
		);
	}

	if (JumpscareCollision)
	{
		JumpscareCollision->OnComponentBeginOverlap.AddDynamic(
			this,
			&AMonsterCharacter::OnJumpscareCollisionBeginOverlap
		);
	}

	if (GameOverPostProcessVolume)
	{
		GameOverPostProcessVolume->bUnbound = false;
	}

	if (bIsEscaping) return;

	UpdateBlackboard();
}

void AMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (TargetActor && bCanChase && !bIsEscaping)
	//{
	//	if (!IsPlayerOnNavMesh(TargetActor))
	//	{
	//		//bCanChase = false;
	//		TargetActor = nullptr;

	//		UpdateBlackboard();
	//	}
	//}
}

void AMonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMonsterCharacter::OnDetectSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!OtherActor)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (OtherActor != PlayerPawn)
	{
		return;
	}

	if (bHasDetectedPlayer)
	{
		return;
	}

	if (CanSeePlayer(OtherActor))
	{
		bIsIdle = false;
		bHasDetectedPlayer = true;
		bIsLookingUp = true;
		bCanChase = false;
		TargetActor = OtherActor;
	}

	UpdateBlackboard();
}

void AMonsterCharacter::OnJumpscareCollisionBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!OtherActor)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (OtherActor != PlayerPawn)
	{
		return;
	}

	StartJumpscare(OtherActor);
}

void AMonsterCharacter::FinishLookUp()
{
	if (!bHasDetectedPlayer)
	{
		return;
	}

	if (bIsEscaping) return;

	bIsLookingUp = false;
	bCanChase = true;

	UpdateBlackboard();
}

void AMonsterCharacter::UpdateBlackboard()
{
	AMonsterAIController* MonsterAIController = Cast<AMonsterAIController>(GetController());

	if (!MonsterAIController)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = MonsterAIController->GetBlackboardComponent();

	if (!BlackboardComp)
	{
		return;
	}

	BlackboardComp->SetValueAsObject(TEXT("TargetActor"), TargetActor);
	BlackboardComp->SetValueAsBool(TEXT("bHasDetectedPlayer"), bHasDetectedPlayer);
	BlackboardComp->SetValueAsBool(TEXT("bIsLookingUp"), bIsLookingUp);
	BlackboardComp->SetValueAsBool(TEXT("bCanChase"), bCanChase);
	BlackboardComp->SetValueAsBool(TEXT("bIsEscaping"), bIsEscaping);
	BlackboardComp->SetValueAsBool(TEXT("bIsIdle"), bIsIdle);
}

void AMonsterCharacter::StartJumpscare(AActor* PlayerActor)
{
	if (!bCanChase || bIsJumpscaring || !PlayerActor)
	{
		return;
	}

	bIsJumpscaring = true;

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		APawn* PlayerPawn = PC->GetPawn();

		if (PlayerPawn)
		{
			PlayerPawn->DisableInput(PC);
		}

		if (JumpscareCamera)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			ACameraActor* TempCameraActor = GetWorld()->SpawnActor<ACameraActor>(
				ACameraActor::StaticClass(),
				JumpscareCamera->GetComponentLocation(),
				JumpscareCamera->GetComponentRotation(),
				SpawnParams
			);

			if (TempCameraActor)
			{
				PC->SetViewTargetWithBlend(TempCameraActor, 0.1f);
			}
		}
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && JumpscareMontage)
	{
		AnimInstance->Montage_Play(JumpscareMontage);

		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &AMonsterCharacter::OnJumpscareMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, JumpscareMontage);

	}
}

void AMonsterCharacter::OnJumpscareMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != JumpscareMontage)
	{
		return;
	}

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();
		AICon->BrainComponent->StopLogic(TEXT("Jumpscare End"));
	}

	GetCharacterMovement()->DisableMovement();

	GetMesh()->bPauseAnims = true;

	if (GameOverPostProcessVolume)
	{
		GameOverPostProcessVolume->bUnbound = true;
	}

	AProject_SmileCharacter* Player = Cast<AProject_SmileCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Player)
	{
		Player->GameOver();
	}
}

bool AMonsterCharacter::IsPlayerOnNavMesh(AActor* PlayerActor) const
{
	if (!PlayerActor)
	{
		return false;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	if (!NavSystem)
	{
		return false;
	}

	FNavLocation ProjectedLocation;

	return NavSystem->ProjectPointToNavigation(
		PlayerActor->GetActorLocation(),
		ProjectedLocation,
		FVector(100.0f, 100.0f, 300.0f)
	);
}

bool AMonsterCharacter::CanSeePlayer(AActor* PlayerActor)
{
	if (!PlayerActor) return false;

	FVector Start = GetActorLocation() + FVector(0, 0, 50.f);

	FVector End = PlayerActor->GetActorLocation() + FVector(0, 0, 50.f);

	FHitResult HitResult;

	FCollisionQueryParams Params;

	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (!bHit) return false;

	return HitResult.GetActor() == PlayerActor;
}

void AMonsterCharacter::StartEscapeSequence()
{
	if (bIsEscaping)
	{
		return;
	}

	bCanChase = false;
	bIsLookingUp = false;
	bHasDetectedPlayer = false;
	bIsEscaping = false;
	TargetActor = nullptr;

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();
	}

	GetCharacterMovement()->StopMovementImmediately();

	UpdateBlackboard();

	GetWorldTimerManager().SetTimer(
		EscapeTimerHandle,
		this,
		&AMonsterCharacter::SetBlackboardTargetToEscapePoint,
		EscapeWaitTime,
		false
	);
}

void AMonsterCharacter::SetBlackboardTargetToEscapePoint()
{
	if (!EscapeTargetPoint)
	{
		return;
	}

	bIsIdle = false;
	bCanChase = false;
	bIsLookingUp = false;
	bHasDetectedPlayer = false;
	bIsEscaping = true;

	TargetActor = EscapeTargetPoint;

	UpdateBlackboard();
}

void AMonsterCharacter::FinishEscape()
{
	bIsIdle = true;
	bIsEscaping = false;
	bCanChase = false;
	bIsLookingUp = false;
	bHasDetectedPlayer = false;
	TargetActor = nullptr;

	UpdateBlackboard();
}