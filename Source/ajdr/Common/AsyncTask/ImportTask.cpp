
#include "ajdr.h"
#include "ImportTask.h"
#include "HttpModule.h"
#include "ModelFile.h"
#include "ModelImporter.h"
#include "Interfaces/IHttpRequest.h"

//////////////////////////////////////////////////////////////////////////
FImportAsyncTask::FImportAsyncTask(UModelFile *InModelFile, UModelFile *InUpdateModelFile, UModelImporter *InImporter, const FString &InFilename)
{
	Filename = InFilename;
	Importer = InImporter;
	
	ModelFile = InModelFile;
	UpdateModelFile = InUpdateModelFile;

// 	CompoundModelFile = NULL;
// 	UpdateCompoundModelFile = NULL;
}

FImportAsyncTask::FImportAsyncTask(UCompoundModelFile *InModelFile, UCompoundModelFile *InUpdateModelFile, UModelImporter *InImporter, const FString &InFilename)
{
	ModelFile = NULL;
	UpdateModelFile = NULL;

	Filename = InFilename;
	Importer = InImporter;

// 	CompoundModelFile = InModelFile;
// 	UpdateCompoundModelFile = InUpdateModelFile;
}

void FImportAsyncTask::DoWork()
{
	if (Importer)
	{
		if (ModelFile)
		{
			Importer->Import(ModelFile, Filename);
		}
// 		else if(CompoundModelFile)
// 		{
// 			Importer->ImportCompoundModel(CompoundModelFile, Filename);
// 		}
	}
}

void FImportAsyncTask::ExecuteDone()
{
	UResource *ImportRes = ModelFile;
	UResource *UpdateRes = UpdateModelFile;

// 	if (!ImportRes)
// 	{
// 		ImportRes = CompoundModelFile;
// 		UpdateRes = UpdateCompoundModelFile;
// 	}

	if (Importer && ModelFile)
	{
		Importer->RedirectMaterialPath(ModelFile);
	}
	
	if (ImportRes)
	{
		if (UpdateModelFile)
		{
			FString Resid = UpdateModelFile->GetResID();
			UpdateModelFile->ForceLoad();
			UpdateModelFile->CloneModel(ModelFile);
			UpdateModelFile->GetSummary()->ResID =Resid;
			UpdateModelFile->SetSizeInMM(ModelFile->DepthInMM, ModelFile->WidthInMM, ModelFile->HeightInMM);
			ImportRes->ConditionalBeginDestroy();
			ImportRes = NULL;
			UpdateModelFile->MarkDirty(true);
		}
		else
		{
			ImportRes->MarkDirty();
			ImportRes->OnPostLoad();
			ImportRes->SetState(EResState::EResLoaded);
		}
	}
}

void FImportAsyncTask::AddReferencedObjects(FReferenceCollector& Collector)
{
// 	if (CompoundModelFile)
// 	{
// 		Collector.AddReferencedObject(CompoundModelFile);
// 	}
// 
// 	if (UpdateCompoundModelFile)
// 	{
// 		Collector.AddReferencedObject(UpdateCompoundModelFile);
// 	}

	if (ModelFile)
	{
		Collector.AddReferencedObject(ModelFile);
	}

	if (UpdateModelFile)
	{
		Collector.AddReferencedObject(UpdateModelFile);
	}

	Collector.AddReferencedObject(Importer);
}


