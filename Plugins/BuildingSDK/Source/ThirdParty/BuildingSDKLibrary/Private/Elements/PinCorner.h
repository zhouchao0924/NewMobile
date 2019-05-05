
#pragma once

#include "kLine.h"
#include "Corner.h"

class PinCorner : public Corner
{
	DEFIN_CLASS()
public:
	PinCorner();
	EObjectType GetType() { return EPinCorner; }
	bool IsDeletable() { return false; }
	bool IsFree() override;
	void Update() override;
public: 
	ObjectID	OwnerID;
};


