
#pragma once

#include "Math/Box.h"
#include "Math/kBox.h"
#include "Math/kVector2D.h"
#include "Math/kVector3D.h"

FVector2D ToVector2D(const kPoint &pt);
FVector   ToUE4Vector(const kVector3D &Loc);
FVector   ToUE4Position(const kVector3D &Loc);
FRotator  ToUE4Rotation(const kRotation &Rotation);
FBox	  ToUE4Bounds(const kBox3D &Bounds);
FPlane	  ToUE4Plane(const kPlane3D &Plane);
kVector3D ToBuildingPosition(const FVector &V);
kPoint	  ToBuildingPosition(const FVector2D &V);
kVector3D ToBuildingVector(const FVector &V);
kPoint	  ToBuildingVector(const FVector2D &V);
kBox3D	  ToBuildingBox(const FBox &V);
float	  ToBuildingLen(float UE4Len);
float	  ToUE4Len(float SuiteLen);



