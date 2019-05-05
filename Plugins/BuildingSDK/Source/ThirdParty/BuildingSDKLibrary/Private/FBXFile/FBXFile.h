
#pragma once

#include "IMeshObject.h"
#include "ISerialize.h"
#include "Class/Object.h"
#include "Mesh/MeshObject.h"
#include "Mesh/SurfaceObject.h"

class FBXFile :public Object, public IMeshObjectCallback
{
public:
	static FBXFile *LoadFromFile(const char *InFilename);
	ISurfaceObject *GetMeshSurface(int SurfaceID) override;
	void Link(int SurfaceID, int SubModelIndex) override;
	void UnLink(int SurfaceID, int SubModelIndex) override;
protected:
	std::string		_fbxFilename;
};


