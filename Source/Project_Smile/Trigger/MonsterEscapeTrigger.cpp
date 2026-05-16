// Fill out your copyright notice in the Description page of Project Settings.


#include "Trigger/MonsterEscapeTrigger.h"
#include "Components/BoxComponent.h"

#include "Character/MonsterCharacter.h"


// Sets default values
AMonsterEscapeTrigger::AMonsterEscapeTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called when the game starts or when spawned
void AMonsterEscapeTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMonsterEscapeTrigger::OnTriggerBeginOverlap);
	}
	
}

// Called every frame
void AMonsterEscapeTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMonsterEscapeTrigger::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	AMonsterCharacter* MonsterCharacter = Cast<AMonsterCharacter>(OtherActor);

	if (!MonsterCharacter) return;

	MonsterCharacter->StartEscapeSequence();
}

