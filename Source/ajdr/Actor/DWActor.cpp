
#include "ajdr.h"
#include "Building/BuildingData.h"
#include "DWActor.h"
#include "ResourceMgr.h"
#include "Building/BuildingSystem.h"

ADWActor::ADWActor(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
	DWModelComponentFile = ObjectInitializer.CreateDefaultSubobject<UModelFileComponent>(this, TEXT("ModelFileComponent"));
}

void ADWActor::Update(FString ResID)
{
	UpdateCompoennt(DWModelComponentFile,ResID);
}

void ADWActor::UpdateCompoennt(UModelFileComponent *InModelFileComponent, FString ResID)
{
	if (InModelFileComponent)
	{
		UResourceMgr *ResMgr = UResourceMgr::GetResourceMgr();
		if (ResMgr)
		{
			UModelFile *ModelFile = Cast<UModelFile>(ResMgr->FindRes(ResID));
			if (ModelFile)
			{
				AActor *pActor = Cast<AActor>(InModelFileComponent->GetOwner());
				ModelFile->ForceLoad();
				InModelFileComponent->UpdateModel(ModelFile);
			}
		}
	}
}
