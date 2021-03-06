
#include "Corner.h"
#include "ISuite.h"
#include "kBox.h"
#include "Wall.h"
#include "kLine2D.h"
#include "Class/Property.h"

const float const_minAngle = 30.0f;
const float const_minDist = 40.0f;

void Corner::FWallInfo::Serialize(ISerialize &Ar)
{
	Ar << Angle;
	Ar << WallID;
	KSERIALIZE_ENUM(EObjectType, WallType);
}

BEGIN_CLASS(Corner)
	ADD_PROP(Location, Vec2DProperty)
END_CLASS()

Corner::Corner()
	: Baton(nullptr)
{
}

void Corner::Serialize(ISerialize &Ar, unsigned int Ver)
{
	BuildingObject::Serialize(Ar, Ver);

	BeginChunk<Corner>(Ar);

	SERIALIZE_COMPLEXVEC(ConnectedWalls);

	EndChunk<Corner>(Ar);
}

void Corner::MarkNeedUpdate(EChannelMask Mask /*= EChannelAll*/)
{
	BuildingObject::MarkNeedUpdate(Mask);

	for (size_t i = 0; i < ConnectedWalls.size(); ++i)
	{
		BuildingObject *pWall = SUITE_GET_BUILDING_OBJ(ConnectedWalls[i].WallID, Wall);
		if (pWall)
		{
			pWall->MarkNeedUpdate();
		}
	}
}

void Corner::AddWall(Wall *pWall)
{
	if (pWall)
	{
		kPoint	 Direction;
		ObjectID CornerID = GetID();

		kPoint P0, P1;
		pWall->GetLocations(P0, P1);

		if (pWall->P[0] == CornerID)
		{
			Direction = P1 - P0;
		}
		else if (pWall->P[1]==CornerID)
		{
			Direction = P0 - P1;
		}

		Direction.Normalize();

		float Degree = Direction.GetAngle();
		size_t n = ConnectedWalls.size();

		ConnectedWalls.resize(n + 1);

		size_t i = n;
		for (; i > 0; --i)
		{
			if (ConnectedWalls[i - 1].Angle > Degree)
			{
				ConnectedWalls[i] = ConnectedWalls[i - 1];
			}
			else
			{
				break;
			}
		}
		ConnectedWalls[i].Angle = Degree;
		ConnectedWalls[i].WallID = pWall->GetID();
		ConnectedWalls[i].WallType = pWall->GetType();

		MarkNeedUpdate();
	}
}

void Corner::SetLocation(const kPoint &Loc)
{
	Location = Loc;
	MarkNeedUpdate();
}
	
void Corner::GetNearByWall(ObjectID WallID, ObjectID &LeftWall, ObjectID &RightWall, bool bIgnoreVirtualWall)
{
	LeftWall = RightWall = INVALID_OBJID;

	int l_index, r_index;
	GetNearByWallIndex(WallID, l_index, r_index, bIgnoreVirtualWall);
		
	if (l_index >= 0)
	{
		LeftWall = ConnectedWalls[l_index].WallID;
	}

	if (r_index >= 0)
	{
		RightWall = ConnectedWalls[r_index].WallID;
	}
}

int Corner::GetWallIndex(ObjectID WallID)
{
	int foundIndex = -1;
	int n = (int)ConnectedWalls.size();

	for (int i = 0; i < n; ++i)
	{
		if (ConnectedWalls[i].WallID == WallID)
		{
			foundIndex = i;
			break;
		}
	}

	return foundIndex;
}

void Corner::GetNearByWallIndex(ObjectID WallID, int &LeftWall, int &RightWall, bool bIgnoreVirtualWall)
{
	LeftWall = RightWall = -1;

	int n = (int)ConnectedWalls.size();
	if (n > 1)
	{
		for (size_t i = 0; i < n; ++i)
		{
			if (ConnectedWalls[i].WallID == WallID)
			{
				for (size_t k = 1; k < ConnectedWalls.size(); ++k)
				{
					int index = (i - k + n) % n;
					if (!bIgnoreVirtualWall || ConnectedWalls[index].WallType != EVirtualWall)
					{
						RightWall = index;
						break;
					}
				}
				for (size_t k = 1; k < ConnectedWalls.size(); ++k)
				{
					int index = (i + k + n) % n;
					if (!bIgnoreVirtualWall || ConnectedWalls[index].WallType != EVirtualWall)
					{
						LeftWall = index;
						break;
					}
				}
			}
		}
	}
}

bool Corner::IsConnect(ObjectID CornerID)
{
	for (size_t i = 0; i < ConnectedWalls.size(); ++i)
	{
		Wall *pWall = SUITE_GET_BUILDING_OBJ(ConnectedWalls[i].WallID, Wall);
		if (pWall)
		{
			if (CornerID == pWall->GetOtherCorner(_ID))
			{
				return true;
			}
		}
	}
	return false;
}

void Corner::RemoveWall(ObjectID WallID)
{
	if (WallID != INVALID_OBJID)
	{
		for (size_t i = 0; i < ConnectedWalls.size(); ++i)
		{
			if (ConnectedWalls[i].WallID == WallID)
			{
				ConnectedWalls.erase(ConnectedWalls.begin() + i);
				MarkNeedUpdate();
				break;
			}
		}
	}
}

bool Corner::IsFree()
{
	return ConnectedWalls.size() == 0;
}

bool Corner::IsValid()
{
	int iMin = 0;
	std::vector<float> newConnAngles;
	RecalcAngles(newConnAngles, nullptr);
	return IsValid(newConnAngles, iMin);
}

bool Corner::IsWallDistValid()
{
	if (_Suite)
	{
		const float const_MinDistSQ = const_minDist*const_minDist;
		IObject ** ppWalls = nullptr;
		int nWall = _Suite->GetAllObjects(ppWalls, EWall);
			
		std::vector<kLine2D> ConnWallLines;
		for (size_t i = 0; i < ConnectedWalls.size(); ++i)
		{
			Wall *pWall = SUITE_GET_BUILDING_OBJ(ConnectedWalls[i].WallID, Wall);
			if (pWall)
			{
				kPoint P0, P1;
				pWall->GetLocations(P0, P1);
				ConnWallLines.push_back(kLine2D(P0, P1));
			}
		}

		for (int i = 0; i < nWall; ++i)
		{
			Wall *pWall = (Wall *)ppWalls[i];
			if (pWall->P[0] == _ID || pWall->P[1] == _ID)
			{
				continue;
			}

			kPoint P0, P1;
			pWall->GetLocations(P0, P1);

			kPoint Vec = P1 - P0;
			float Len = Vec.Size();
			if (Len > 0)
			{
				kPoint N = Vec / Len;
				kPoint Dir = Location - P0;
				float fDot = Dir.Dot(N);

				float DistSQ = 0;
				if (fDot < 0)
				{
					DistSQ = Dir.SizeSquared();
				}
				else if (fDot > Len)
				{
					DistSQ = (Location - P1).SizeSquared();
				}
				else
				{
					kPoint ProjP = P0 + N*fDot;
					DistSQ = (Location - ProjP).SizeSquared();
				}

				if (DistSQ < const_MinDistSQ)
				{
					return false;
				}

				float Ratio = 0;
				for (size_t k = 0; k < ConnWallLines.size(); ++k)
				{
					if (ConnWallLines[k].GetIntersection(kLine2D(P0, P1), Ratio) && Ratio> 0 && Ratio<1.0f)
					{
						return false;
					}
				}
			}
		}

		return true;
	}

	return false;
}

bool Corner::Move(const kPoint &DeltaMove)
{
	kPoint SavedLoc = Location;
	Location += DeltaMove;

	std::vector<float>	newAngles;
	std::vector<ObjectID> connCorners;

	RecalcAngles(newAngles, &connCorners);

	int iMin = 0, n = (int)ConnectedWalls.size();
	if (!IsValid(newAngles, iMin)) // self valid check
	{
		Location = SavedLoc;
		return false;
	}

	for (size_t i = 0; i < connCorners.size(); ++i) //connnect corner valid check
	{
		Corner *pConnCorner = SUITE_GET_BUILDING_OBJ(connCorners[i], Corner);
		if (!pConnCorner || !pConnCorner->IsValid())
		{
			Location = SavedLoc;
			return false;
		}
	}
		
	//intersection check
// 	if (!IsWallDistValid())
// 	{
// 		Location = SavedLoc;
// 		return false;
// 	}

	//resort connnect wall
	std::vector<FWallInfo> newConnectWalls;
	newConnectWalls.resize(n);

	for (int i = 0; i < n; i++)
	{
		newConnectWalls[i] = ConnectedWalls[(i + iMin) % n];
		newConnectWalls[i].Angle = newAngles[(i + iMin) % n];
	}

	ConnectedWalls.swap(newConnectWalls);

	for (size_t i = 0; i < ConnectedWalls.size(); ++i)
	{
		Wall *pWall = SUITE_GET_BUILDING_OBJ(ConnectedWalls[i].WallID, Wall);
		if (pWall)
		{
			ObjectID IDCon = pWall->GetOtherCorner(_ID);
			Corner *pCornerCon = SUITE_GET_BUILDING_OBJ(IDCon, Corner);
			if(pCornerCon)
			{
				pCornerCon->MarkNeedUpdate();
			}
		}
	}

	MarkNeedUpdate();

	return true;
}

IValue *Corner::GetFunctionProperty(const std::string &name)
{
	IValue *pValue = BuildingObject::GetFunctionProperty(name);

	if (!pValue)
	{
		if (name == "Walls")
		{
			static std::vector<ObjectID> Walls;

			size_t nWall = ConnectedWalls.size();
			Walls.resize(nWall);

			for (size_t i = 0; i < nWall; ++i)
			{
				Walls[i] = ConnectedWalls[i].WallID;
			}

			pValue = &GValueFactory->Create(kArray<int>(Walls));
		}
	}

	return pValue;
}

void Corner::RecalcAngles(std::vector<float> &newAngles, std::vector<ObjectID> *connCorners)
{
	size_t n = ConnectedWalls.size();
	newAngles.resize(n);

	if (connCorners)
	{
		connCorners->resize(n);
	}

	kPoint Direction;
	for (size_t i = 0; i < ConnectedWalls.size(); ++i)
	{
		kPoint P0, P1;

		Wall *pWall = SUITE_GET_BUILDING_OBJ(ConnectedWalls[i].WallID, Wall);
		pWall->GetLocations(P0, P1);

		if (pWall->P[0] == _ID)
		{
			Direction = P1 - P0;
			if (connCorners)
			{
				(*connCorners)[i] = pWall->P[1];
			}
		}
		else if (pWall->P[1] == _ID)
		{
			Direction = P0 - P1;
			if (connCorners)
			{
				(*connCorners)[i] = pWall->P[0];
			}
		}

		Direction.Normalize();
		float Degree = Direction.GetAngle();
		newAngles[i] = Degree;
	}
}

bool Corner::IsValid(std::vector<float> &newAngles, int &iMin)
{
	iMin = 0;
	float minAngle = 400.0f;
	for (size_t i = 0; i < newAngles.size(); ++i)
	{
		if (newAngles[i] < minAngle)
		{
			minAngle = newAngles[i];
			iMin = i;
		}
	}

	float delta = 0;
	int n = newAngles.size();
	if (n > 1)
	{
		float preAngle = newAngles[iMin];
		for (int i = 1; i < n; ++i)
		{
			float curAngle = newAngles[(i + iMin) % n];
			delta = curAngle - preAngle;

			if (delta < 0) //角点相连的墙体穿插检测
			{
				return false;
			}

			if (delta < const_minAngle) //角度限制检测
			{
				return false;
			}

			preAngle = curAngle;
		}

		delta = newAngles[iMin] - preAngle;
		assert(delta < 0);

		if (-delta < const_minAngle)
		{
			return false;
		}
	}

	return true;
}


