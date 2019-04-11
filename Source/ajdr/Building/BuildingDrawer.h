
#pragma once

#include "ISuite.h"
#include "DRDrawer.h"

class FBuildingDrawer
{
public:
	static void DrawCorner(FDrawer *Drawer, IObject *pCorner);
	static void DrawRoom(FDrawer *Drawer, IObject *pRoom);
	static void DrawWall(FDrawer *Drawer, IObject *pWall, bool bSelected);
	static void DrawDoor(FDrawer *Drawer, IObject *pDoor, bool bSelected);
	static void DrawDoor(FDrawer *Drawer, bool bSelected, const FVector2D &Loc, const FVector2D &Forward, const FVector2D &Right, float Width);
	static void DrawWindow(FDrawer *Drawer, IObject *pWindow, bool bSelected);
	static void DrawWindow(FDrawer *Drawer, bool bSelected, const FVector2D &Loc, const FVector2D &Forward, const FVector2D &Right, float Width);
	static FLinearColor GetColor(bool bSelected);
protected:
	static void BeginObject(FDrawer *Drawer, IObject *Obj);
	static void EndObject(FDrawer *Drawer);
protected:
	static float	WallThickness;
};

