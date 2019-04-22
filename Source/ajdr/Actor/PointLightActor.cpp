
#include "ajdr.h"
#include "Building/BuildingData.h"
#include "PointLightActor.h"
#include "Building/BuildingSystem.h"

APointLightActor::APointLightActor(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
	PointLightComponent = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, TEXT("PointLightComponent"));
}

void APointLightActor::Update(UBuildingData *Data,float size)
{	
	float SourceRadius = Data->GetFloat(TEXT("SourceRadius"));
	float SoftSourceRadius = Data->GetFloat(TEXT("SoftSourceRadius"));
	float  SourceLength = Data->GetFloat(TEXT("SourceLength"));
	float  Intensity = Data->GetFloat(TEXT("Intensity"));
	FLinearColor LightColor;
	bool bCastShadow = Data->GetBool(TEXT("bCastShadow"));

	UPointLightComponent* PLC = NewObject<UPointLightComponent>();
	
	PLC->RegisterComponentWithWorld(GetWorld());
	PLC->AttachToComponent(PointLightComponent, FAttachmentTransformRules::KeepRelativeTransform);

	PLC->SetCastShadows(bCastShadow);
	PLC->bUseInverseSquaredFalloff = false;
	PLC->SetIntensity(size);
	PLC->SetLightColor(FLinearColor(199, 125, 108));
	PLC->SetAttenuationRadius(SourceRadius);
}

