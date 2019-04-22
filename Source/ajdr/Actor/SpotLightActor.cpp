
#include "ajdr.h"
#include "Building/BuildingData.h"
#include "SpotLightActor.h"
#include "Building/BuildingSystem.h"

ASpotLightActor::ASpotLightActor(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
	SpotLightComponent = ObjectInitializer.CreateDefaultSubobject<USpotLightComponent>(this, TEXT("PointLightComponent"));
}

void ASpotLightActor::Update(UBuildingData *Data, float size)
{	
	float SourceRadius = Data->GetFloat(TEXT("SourceRadius"));
	float SoftSourceRadius = Data->GetFloat(TEXT("SoftSourceRadius"));
	float  SourceLength = Data->GetFloat(TEXT("SourceLength"));
	float  Intensity = Data->GetFloat(TEXT("Intensity"));
	FLinearColor LightColor;
	bool bCastShadow = Data->GetBool(TEXT("bCastShadow"));

	USpotLightComponent* SLC = NewObject<USpotLightComponent>();
	
	SLC->RegisterComponentWithWorld(GetWorld());
	SLC->AttachToComponent(SpotLightComponent, FAttachmentTransformRules::KeepRelativeTransform);

	SLC->SetCastShadows(bCastShadow);
	SLC->bUseInverseSquaredFalloff = false;
	SLC->SetIntensity(size);
	SLC->SetLightColor(FLinearColor(199, 125, 108));
	SLC->SetAttenuationRadius(SourceRadius);
}

