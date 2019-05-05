
#pragma once

#include <vector>
#include "Class/Object.h"

class ISuite;
class Corner;
class Anchor;
class BuildingObject : public Object
{
	DEFIN_CLASS()
public:
	BuildingObject();
	~BuildingObject() {}
	ISuite *GetSuite() { return _Suite; }
	void Serialize(ISerialize &Ar, unsigned int Ver)override;
	EObjectType GetType() override { return EBuildingObject; }
	virtual bool GetBox2D(kPoint &Orignal, kPoint &Range) { return false; }
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual void GetCorners(std::vector<Corner *> &Corners) {};
	void MarkNeedUpdate(EChannelMask Mask = EChannelAll) override;
	void SetID(ObjectID InID) { _ID = InID; }
	virtual Anchor *CreateAnchor() { return nullptr; }		
	virtual void Delete() override;
	friend class Anchor;
protected:
	friend class		  SuiteImpl;
	std::vector<ObjectID> AnchorSlots;
	class ISuite		 *_Suite;
};


