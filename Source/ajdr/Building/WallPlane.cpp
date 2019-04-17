// Copyright? 2017 ihomefnt All Rights Reserved.

#include "ajdr.h"
#include "WallPlane.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"

// Sets default values
AWallPlaneBase::AWallPlaneBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Plane = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Wall2dMesh"));
	Plane->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	LeftLabelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LeftLabelWidget"));
	LeftLabelWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	RightLabelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("RightLabelWidget"));
	RightLabelWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AWallPlaneBase::BeginPlay()
{
	Super::BeginPlay();
	TrianglesID = { 0,1,2,0,2,3,0,3,4,0,4,5 };
	for (int i = 0; i < 12; ++i)
	{
		NormalsDir.Add(FVector(0, 0, 1));
	}
	WallID = -1;
}

// Called every frame
void AWallPlaneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWallPlaneBase::InitPlane(const TArray<FVector> _TArrayPos, float ZPos , int32 ObjectID ,bool bgeneratecollision)
{
	if (bgeneratecollision)
	{
		LeftLabelWidget->SetVisibility(false);
		RightLabelWidget->SetVisibility(false);
	}
	bGeneratecollision = bgeneratecollision;
	VerListPos.Empty();
	for (int i = 0; i < _TArrayPos.Num(); ++i)
	{
		VerListPos.Add(FVector(_TArrayPos[i].X, _TArrayPos[i].Y, ZPos));
	}
	if (VerListPos.Num() > 2)
	{
		WallID = ObjectID;
		Createplane();
	}
}

void AWallPlaneBase::SetRuler(bool IsShow)
{
	if (LeftLabelWidget&&RightLabelWidget)
	{
		LeftLabelWidget->SetVisibility(IsShow);
		RightLabelWidget->SetVisibility(IsShow);
	}
}