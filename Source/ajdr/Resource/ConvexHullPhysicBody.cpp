
#include "ajdr.h"
#include "ModelCommons.h"
#include "ModelFile.h"
#include "ConvexHullPhysicBody.h"

// Only enabling on windows until other platforms can test!
#define USE_VHACD  (PLATFORM_WINDOWS || PLATFORM_LINUX || PLATFORM_MAC)

#include "Misc/FeedbackContext.h"
#include "PhysicsEngine/ConvexElem.h"

#if USE_VHACD

#include "ThirdParty/VHACD/public/VHACD.h"

#if PLATFORM_MAC
#include <OpenCL/cl.h>
#endif

#else

#ifdef __clang__
// HACD headers use pragmas that Clang doesn't recognize (inline depth)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"	// warning : unknown pragma ignored [-Wunknown-pragmas]
#endif

#include "ThirdParty/HACD/HACD_1.0/public/HACD.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // USE_VHACD

#include "PhysicsEngine/BodySetup.h"

DEFINE_LOG_CATEGORY_STATIC(LogConvexDecompTool, Log, All);

using namespace VHACD;

class FVHACDProgressCallback : public IVHACD::IUserCallback
{
public:
	FVHACDProgressCallback(void) {}
	~FVHACDProgressCallback() {};

	void Update(const double overallProgress, const double stageProgress, const double operationProgress, const char * const stage, const char * const    operation)
	{
		FString StatusString = FString::Printf(TEXT("Processing [%s]..."), ANSI_TO_TCHAR(stage));
	};
};

void DecomposeConvexMeshToHulls(FConvexAggGeom &AggGeom, const TArray<FVector>& InVertices, const TArray<int32>& InIndices, float InAccuracy, int32 InMaxHullVerts)
{
	bool bSuccess = false;

	// Validate input by checking bounding box
	FBox VertBox(EForceInit::ForceInit);
	for (FVector Vert : InVertices)
	{
		VertBox += Vert;
	}

	// If box is invalid, or the largest dimension is less than 1 unit, or smallest is less than 0.1, skip trying to generate collision (V-HACD often crashes...)
	if (VertBox.IsValid == 0 || VertBox.GetSize().GetMax() < 1.f || VertBox.GetSize().GetMin() < 0.1f)
	{
		return;
	}

	FVHACDProgressCallback VHACD_Callback;

	IVHACD::Parameters VHACD_Params;
	VHACD_Params.m_resolution = 1000000; // Maximum number of voxels generated during the voxelization stage (default=100,000, range=10,000-16,000,000)
	VHACD_Params.m_maxNumVerticesPerCH = InMaxHullVerts; // Controls the maximum number of triangles per convex-hull (default=64, range=4-1024)
	VHACD_Params.m_concavity = 0.3f * (1.f - FMath::Clamp(InAccuracy, 0.f, 1.f)); // Maximum allowed concavity (default=0.0025, range=0.0-1.0)
	VHACD_Params.m_callback = &VHACD_Callback;
	VHACD_Params.m_oclAcceleration = false;

	IVHACD* InterfaceVHACD = CreateVHACD();

	const float* const Verts = (float*)InVertices.GetData();
	const unsigned int NumVerts = InVertices.Num();
	const uint32_t* const Tris = (uint32_t*)InIndices.GetData();
	const unsigned int NumTris = InIndices.Num() / 3;

	bSuccess = InterfaceVHACD->Compute(Verts, NumVerts, Tris, NumTris, VHACD_Params);

	if (bSuccess)
	{
		int32 NumHulls = InterfaceVHACD->GetNConvexHulls();

		for (int32 HullIdx = 0; HullIdx < NumHulls; HullIdx++)
		{
			// Create a new hull in the aggregate geometry
			FKConvexElem ConvexElem;

			IVHACD::ConvexHull Hull;
			InterfaceVHACD->GetConvexHull(HullIdx, Hull);
			for (uint32 VertIdx = 0; VertIdx < Hull.m_nPoints; VertIdx++)
			{
				FVector V;
				V.X = (float)(Hull.m_points[(VertIdx * 3) + 0]);
				V.Y = (float)(Hull.m_points[(VertIdx * 3) + 1]);
				V.Z = (float)(Hull.m_points[(VertIdx * 3) + 2]);

				ConvexElem.VertexData.Add(V);
			}


			ConvexElem.UpdateElemBox();
			AggGeom.ConvexElems.Add(ConvexElem);
		}
	}

	InterfaceVHACD->Clean();
	InterfaceVHACD->Release();
}

void GenerateModelFileConvexHull(UModelFile *ModelFile, float Accuracy, int32 MaxHullVerts, bool bCleanExsit)
{
	TArray<FModel *> SubModels = ModelFile->GetSubModels();

	for (int32 i = 0; i < SubModels.Num(); ++i)
	{
		FModel *Model = SubModels[i];
		if (Model && (bCleanExsit || Model->AggGeom.ConvexElems.Num() == 0))
		{
			GenerateModelConvexHull(Model, Accuracy, MaxHullVerts);
		}
	}
}
void GenerateModelConvexHull(FModel *Model, float Accuracy, int32 MaxHullVerts)
{
	if (Model)
	{
		Model->AggGeom.ConvexElems.Empty();

		if (Model->UCXData.Indices.Num() > 0)
		{

			DecomposeConvexMeshToHulls(Model->AggGeom, Model->UCXData.Vertices, Model->UCXData.Indices, Accuracy, MaxHullVerts);

		}
		else
		{
			int32 Offset = 0, nVtx = 0, nIndices = 0;
			TArray<FVector> Vertices;
			TArray<int32>  Indices;


			for (int32 iSection = 0; iSection < Model->Sections.Num(); ++iSection)
			{
				FProcMeshSection &Section = Model->Sections[iSection];
				nVtx += Section.ProcVertexBuffer.Num();
				nIndices += Section.ProcIndexBuffer.Num();
			}

			Vertices.SetNum(nVtx);
			Indices.SetNum(nIndices);

			FVector *pVtx = Vertices.GetData();
			int32 *pIndices = Indices.GetData();

			for (int32 iSection = 0; iSection < Model->Sections.Num(); ++iSection)
			{
				FProcMeshSection &Section = Model->Sections[iSection];

				for (int32 iVert = 0; iVert < Section.ProcVertexBuffer.Num(); ++iVert)
				{
					*pVtx = Section.ProcVertexBuffer[iVert].Position;
					++pVtx;
				}

				for (int32 Index = 0; Index < Section.ProcIndexBuffer.Num(); ++Index)
				{
					*pIndices = Section.ProcIndexBuffer[Index] + Offset;
					++pIndices;
				}

				Offset += Section.ProcVertexBuffer.Num();
			}

			DecomposeConvexMeshToHulls(Model->AggGeom, Vertices, Indices, Accuracy, MaxHullVerts);
		}
	}
}

