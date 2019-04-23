

#include "ajdr.h"
#include "BuildingConfig.h"
#include "IMeshObject.h"

FBuildingConfig::FBuildingConfig()
	: WallMaterial(LoadObject<UMaterialInstance>(nullptr, TEXT("MaterialInstanceConstant'/Game/Common/DefaultWallMaterial_Inst.DefaultWallMaterial_Inst'")))
	, WallMaterialType(EUE4Material)
	, FloorMaterial(LoadObject<UMaterialInstance>(nullptr, TEXT("MaterialInstanceConstant'/Game/_Basic/Materials/Root/M_WallArea_Inst.M_WallArea_Inst'")))
	, FloorMaterialType(EUE4Material)
	, CeilMaterial(LoadObject<UMaterialInstance>(nullptr, TEXT("MaterialInstanceConstant'/Game/Common/DefaultWallMaterial_Inst.DefaultWallMaterial_Inst'")))
	, CeilMaterialType(EUE4Material)
	, bCeilVisible(false)
	, Tolerance(20.f)
	, SkirtingCeil(TEXT("F048F77D44EB875E9DEC2B8BAB9C0E5D"))
	, SkirtingCeilExt(FVector(0.f, 0.f, 0.f))
	, SkirtingFloor(TEXT("5B38B7CE4E8816B679BE33BA1DE448DC"))
	, SkirtingFloorExt(FVector(0.f,0.f,0.f))
	//, DefaultDoor(TEXT("956FA3DC4408FCFE875CD58D4D68D461"))
	//, DefaultDoorFrame(TEXT("83EDE38B4CC505CC6DABA682AB46D32F"))
	//, DefaultWindow(TEXT("DF61B94D45A60B006D43B68FC0E03552"))
{
	
}

