
#include "assert.h"
#include "IBuildingSDK.h"
#include "FBXFile/FBXFile.h"
#include "Mesh/MeshObject.h"
#include "Stream/FileStream.h"
#include "DataExchange/FBXImportImpl.h"

FBXFile *FBXFile::LoadFromFile(const char *InFilename)
{
	if (InFilename)
	{
		FBXImporterImpl *Importer = FBXImporterImpl::Get();
		if (Importer)
		{
			return Importer->Import(InFilename);
		}
	}
	return nullptr;
}

ISurfaceObject *FBXFile::GetMeshSurface(int SurfaceID)
{
	return nullptr;
}

void FBXFile::Link(int SurfaceID, int SubModelIndex)
{
}

void FBXFile::UnLink(int SurfaceID, int SubModelIndex)
{
}


