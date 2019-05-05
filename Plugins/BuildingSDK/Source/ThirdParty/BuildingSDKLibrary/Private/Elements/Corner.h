
#pragma once

#include "kLine.h"
#include "BuildingObject.h"

class Wall;
class Corner :public BuildingObject
{
	DEFIN_CLASS()
	struct  FWallInfo
	{
		float		Angle;
		ObjectID	WallID;
		EObjectType WallType;
		void Serialize(ISerialize &Ar);
	};
public:
	Corner();
	void Serialize(ISerialize &Ar, unsigned int Ver);
	EObjectType GetType() { return ECorner; }
	bool IsDeletable() { return true; }
	void GetNearByWall(ObjectID WallID, ObjectID &LeftWall, ObjectID &RightWall, bool bIgnoreVirtualWall);
	int GetWallIndex(ObjectID WallID);
	void SetLocation(const kPoint &Loc);
	const kPoint &GetLocation() { return Location; }
	void MarkNeedUpdate(EChannelMask Mask = EChannelAll) override;
	void AddWall(Wall *pWall);
	void RemoveWall(ObjectID WallID);
	bool IsConnect(ObjectID CornerID);
	virtual bool IsFree();
	void SetBaton(void *InBaton) { Baton = InBaton; }
	void *GetBaton() { return Baton; }
	bool Move(const kPoint &DeltaMove) override;
	IValue *GetFunctionProperty(const std::string &name) override;
protected:
	bool IsWallDistValid();
	void ReSortConnctWalls();
	void GetNearByWallIndex(ObjectID WallID, int &LeftWall, int &RightWall, bool bIgnoreVirtualWall);
	bool IsValid();
	bool IsValid(std::vector<float> &newAngles, int &iMin);
	void RecalcAngles(std::vector<float> &newAngles, std::vector<ObjectID> *connCorners);
public:
	kPoint					Location;
	std::vector<FWallInfo>	ConnectedWalls;
	void					*Baton;
};


