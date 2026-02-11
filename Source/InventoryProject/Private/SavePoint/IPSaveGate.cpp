// Fill out your copyright notice in the Description page of Project Settings.


#include "SavePoint/IPSaveGate.h"

#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/RectLightComponent.h"
#include "Engine/PointLight.h"
#include "Game/IPGameModeBase.h"
#include "Interfaces/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AIPSaveGate::AIPSaveGate()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("CollisionBox"));
	CollisionBox->SetupAttachment(GetRootComponent());
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RectLight = CreateDefaultSubobject<URectLightComponent>(TEXT("RectLight"));
	RectLight->SetupAttachment(GetRootComponent());
	RectLight->SetVisibility(false);
}

void AIPSaveGate::EnableCollision(bool bShouldEnable)
{
	if (bShouldEnable)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		RectLight->SetVisibility(true);
		bTurnOnLight = true;
	}

	else
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RectLight->SetVisibility(false);
		bTurnOnLight = false;
	}
}

void AIPSaveGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RunningTime+= DeltaTime;
	const float SinePeriod = 2*PI/SinePeriodConstant;

	if (RunningTime > SinePeriod)
	{
		RunningTime = 0.f;
	}
	
	if (bTurnOnLight)
	{
		LightGlowing(DeltaTime);
	}
}

void AIPSaveGate::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AIPSaveGate::OnBoxBeginOverlap);

}

void AIPSaveGate::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>()) //플레이어만 통과되도록
	{


		if (AIPGameModeBase* IPGM = Cast<AIPGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			IPGM->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
		}
		
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);

		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}

void AIPSaveGate::LightGlowing(float DeltaTime)
{
	const float Sine = FMath::Abs(BrightnessAmplitude * FMath::Sin(RunningTime * SinePeriodConstant));
	RectLight->SetIntensity(Sine);
}



