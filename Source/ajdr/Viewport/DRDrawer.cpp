
#include "ajdr.h"
#include "DRDrawer.h"

const float constZNear = 90.0f;

FDrawer::FDrawer(ERHIFeatureLevel::Type InFeatureLevel)
	: FeatureLevel(InFeatureLevel)
	, pDI(nullptr)
	, ViewIndex(INDEX_NONE)
	, DynamicBuilder(nullptr)
	, pCanvas(nullptr)
{
}

void FDrawer::DrawPoint(const FVector &P0, float Size, const FLinearColor &Color, float Z)
{
	pDI->DrawPoint(P0, Color, Size, SDPG_World);
}

void FDrawer::DrawLine(const FVector2D &P0, const FVector2D &P1, const FLinearColor &Color, float Z)
{
	FVector Loc0 = FVector(P0, constZNear + Z);
	FVector Loc1 = FVector(P1, constZNear + Z);
	pDI->DrawLine(Loc0, Loc1, Color, SDPG_World);
}

void FDrawer::DrawDotLine(const FVector2D &P0, const FVector2D &P1, const FLinearColor &Color, float Z)
{
	float CurDist = 0;
	float Dist = (P1 - P0).Size();
	const float SegDist = 30.0f;

	FVector Loc0 = FVector(P0, constZNear + Z);
	FVector Loc1 = FVector(P1, constZNear + Z);
	FVector Vec = (Loc1 - Loc0).GetSafeNormal();
	FVector LastLoc = Loc0;

	while (CurDist<Dist)
	{
		FVector R0 = LastLoc;
		FVector R1 = LastLoc + Vec * (SegDist / 2.0f);

		pDI->DrawLine(R0, R1, Color, SDPG_World);

		CurDist += SegDist;
		LastLoc = Loc0 + Vec * CurDist;
	}

	pDI->DrawLine(LastLoc, Loc1, Color, SDPG_World);
}

void FDrawer::DrawLine(FVector2D *P, int32 nPoints, const FLinearColor &Color, float Z)
{
	if (nPoints > 1)
	{
		for (int32 i = 0; i < nPoints - 1; ++i)
		{
			FVector P0 = FVector(P[i], constZNear + Z);
			FVector P1 = FVector(P[i + 1], constZNear + Z);
			pDI->DrawLine(P0, P1, Color, Z);
		}
	}
}

void FDrawer::DrawArc(const FVector2D &Center, const FVector2D &Forward, const FVector2D &Right, float Radius, float StartRad, float EndRad, int32 NumSegment, const FLinearColor &Color, float Z)
{
	if (NumSegment > 2)
	{
		TArray<FVector2D> Points;
		Points.SetNum(NumSegment);

		float rad = (EndRad - StartRad) / NumSegment;
		float rad_cur = StartRad;

		int32 I = 0;
		for (; I < NumSegment; ++I, rad_cur += rad)
		{
			Points[I] = Center + Forward * Radius*cosf(rad_cur) + Right * Radius*sinf(rad_cur);
		}

		DrawLine(Points.GetData(), NumSegment, Color, Z);
	}
}

void FDrawer::DrawBox(const FVector2D &Center, const FVector2D &HalfExt, const FLinearColor &Color, float Z /*= 0*/)
{
	FVector2D V0 = Center + FVector2D(-HalfExt.X, -HalfExt.Y);
	FVector2D V1 = Center + FVector2D(-HalfExt.X,  HalfExt.Y);
	FVector2D V2 = Center + FVector2D( HalfExt.X,  HalfExt.Y);
	FVector2D V3 = Center + FVector2D( HalfExt.X, -HalfExt.Y);

	DrawLine(V0, V1, Color, Z);
	DrawLine(V1, V2, Color, Z);
	DrawLine(V2, V3, Color, Z);
	DrawLine(V3, V0, Color, Z);
}

void FDrawer::DrawCircle(const FVector2D &Center, float Radius, int32 NumSegment, const FLinearColor &Color, float Z /*= 0*/)
{
	const FVector2D Forward(1.0f, 0);
	const FVector2D Right(0, 1.0f);

	TArray<FVector2D> Points;
	Points.SetNum(NumSegment);

	float rad = (2.0f*PI) / NumSegment;
	float rad_cur = 0;

	int32 I = 0;
	for (; I < NumSegment; ++I, rad_cur += rad)
	{
		Points[I] = Center + Forward * Radius*cosf(rad_cur) + Right * Radius*sinf(rad_cur);
	}

	DrawLine(Points.GetData(), NumSegment, Color, Z);
}

int32 FDrawer::AddVertex(const FVector2D &P0)
{
	return DynamicBuilder? DynamicBuilder->AddVertex(FDynamicMeshVertex(FVector(P0,constZNear+MeshZ))) : INDEX_NONE;
}

int32 FDrawer::AddVertex(const FVector &P0)
{
	return DynamicBuilder ? DynamicBuilder->AddVertex(FDynamicMeshVertex(P0)) : INDEX_NONE;
}

void FDrawer::AddTri(int32 v0, int32 v1, int32 v2)
{
	if (DynamicBuilder)
	{
		DynamicBuilder->AddTriangle(v0, v1, v2);
	}
}

void FDrawer::BeginMesh(float InZ)
{
	check(!DynamicBuilder);
	MeshZ = InZ;
	DynamicBuilder = new FDynamicMeshBuilder(FeatureLevel);
}

void FDrawer::EndMesh(const FMatrix &LocalToWorld, FMaterialRenderProxy *MaterialRenderProxy, bool bSelected)
{
	if (DynamicBuilder)
	{
		DynamicBuilder->Draw(pDI, LocalToWorld, MaterialRenderProxy, SDPG_World);
	}
	SAFE_DELETE(DynamicBuilder);
}

void FDrawer::SetHitProxy(HHitProxy *InProxy)
{
	if (pDI)
	{
		pDI->SetHitProxy(InProxy);
	}
}


