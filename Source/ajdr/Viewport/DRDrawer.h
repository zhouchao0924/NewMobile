// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RHIDefinitions.h"
#include "SceneManagement.h"

class FDrawer
{
public:
	FDrawer(ERHIFeatureLevel::Type InFeatureLevel);
	void DrawPoint(const FVector &P0, float Size, const FLinearColor &Color, float Z = 0);
	void DrawLine(const FVector2D &P0, const FVector2D &P1, const FLinearColor &Color, float Z = 0);
	void DrawLine(FVector2D *P0, int32 nPoints, const FLinearColor &Color, float Z = 0);
	void DrawArc(const FVector2D &Center, const FVector2D &Forward, const FVector2D &Right, float Radius, float StartRad, float EndRad, int32 NumSegment, const FLinearColor &Color, float Z = 0);
	void DrawBox(const FVector2D &Center, const FVector2D &HalfExt, const FLinearColor &Color, float Z = 0);
	void DrawCircle(const FVector2D &Center, float Radius, int32 NumSegment, const FLinearColor &Color, float Z = 0);
	void DrawDotLine(const FVector2D &P0, const FVector2D &P1, const FLinearColor &Color, float Z = 0);
	void BeginMesh(float InZ);
	int32 AddVertex(const FVector2D &P0);
	int32 AddVertex(const FVector &P0);
	void AddTri(int32 v0, int32 v1, int32 v2);
	void EndMesh(const FMatrix &LocalToWorld, FMaterialRenderProxy *MaterialRenderProxy, bool bSelected);
	void SetPDI(FPrimitiveDrawInterface *InDI) { pDI = InDI; }
	void SetViewIndex(int32 InViewIndex) { ViewIndex = InViewIndex; }
	void SetHitProxy(HHitProxy *InProxy);
	void SetCanvas(FCanvas *InCanvas) { pCanvas = InCanvas; }
protected:
	FCanvas					*pCanvas;
	FPrimitiveDrawInterface *pDI;
	FDynamicMeshBuilder		*DynamicBuilder;
	ERHIFeatureLevel::Type	FeatureLevel;
	int32					ViewIndex;
	float					MeshZ;
};



