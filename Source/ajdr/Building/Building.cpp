
#include "ajdr.h"
#include "Building.h"

FVector2D ToVector2D(const kPoint &pt) 
{ 
	return FVector2D(pt.X, pt.Y); 
}

FVector ToUE4Vector(const kVector3D &V)
{
	return FVector(V.x, V.y, V.z);
}

FVector ToUE4Position(const kVector3D &V)
{
	return FVector(V.x, V.y, V.z);
}

FRotator ToUE4Rotation(const kRotation &V)
{
	return FRotator(V.Pitch, V.Yaw, V.Roll);
}

FBox ToUE4Bounds(const kBox3D &V)
{
	return FBox(ToUE4Position(V.MinEdge), ToUE4Position(V.MaxEdge));
}

FPlane ToUE4Plane(const kPlane3D &Plane)
{
	return FPlane(ToUE4Position(Plane.Normal), Plane.D);
}

kVector3D ToBuildingPosition(const FVector &V)
{
	return kVector3D(V.X, V.Y, V.Z);
}

kPoint	ToBuildingPosition(const FVector2D &V)
{
	return kPoint(V.X, V.Y);
}

kVector3D ToBuildingVector(const FVector &V)
{
	return kVector3D(V.X, V.Y, V.Z);
}

kPoint	ToBuildingVector(const FVector2D &V)
{
	return kPoint(V.X, V.Y);
}

kBox3D ToBuildingBox(const FBox &V)
{
	return kBox3D(ToBuildingPosition(V.Min), ToBuildingPosition(V.Max));
}

float ToBuildingLen(float UE4Len)
{
	return UE4Len / 100.0f;
}

float ToUE4Len(float SuiteLen)
{
	return SuiteLen * 100.0f;
}





