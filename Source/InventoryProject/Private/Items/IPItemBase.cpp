// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/IPItemBase.h"

#include "CollisionDebugDrawingPublic.h"
#include "Components/IPItemComponent.h"
#include "Components/SphereComponent.h"


AIPItemBase::AIPItemBase()
{
	PrimaryActorTick.bCanEverTick = false;
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SetRootComponent(Sphere);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Sphere);

	ItemComponent = CreateDefaultSubobject<UIPItemComponent>(TEXT("ItemComponent"));
	
}


void AIPItemBase::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AIPItemBase::OnSphereOverlap);
}

void AIPItemBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

