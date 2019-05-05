
#include "DoorHole.h"
#include "Wall.h"
#include "Corner.h"
#include "ISuite.h"
#include "ModelInstance.h"

BEGIN_DERIVED_CLASS(DoorHole, WallHole)
END_CLASS()

DoorHole::DoorHole()
{
}

void DoorHole::Serialize(ISerialize &Ar, unsigned int Ver)
{
	WallHole::Serialize(Ar, Ver);
}

void DoorHole::SetModel(const std::string &Door, const std::string &DoorFrame, const std::string &DoorSone)
{
	if (DoorFrame.length() > 0)
	{
		ObjectID ID = _Suite->AddModelToAnchor(GetID(), DoorFrame.c_str(), kVector3D(), kRotation(), kVector3D(1.0f), 0);
		if (ID != INVALID_OBJID)
		{
			if (Door.length() > 0)
			{
				_Suite->AddModelToObject(ID, Door.c_str(), kVector3D(), kRotation(), kVector3D(1.0f), 1);
			}

			if (DoorSone.length() > 0)
			{
				_Suite->AddModelToObject(ID, DoorSone.c_str(), kVector3D(), kRotation(), kVector3D(1.0f), 2);
			}
		}
	}
}

IValue *DoorHole::GetFunctionProperty(const std::string &name)
{
	IValue *pValue = WallHole::GetFunctionProperty(name);
	
	if (!pValue)
	{
		if (name == "DoorFrame")
		{
			ModelInstance *pModel = GetModelByType(0);
			if (pModel)
			{
				pValue = &GValueFactory->Create(pModel->GetID());
			}
		}
		else if (name == "Door")
		{
			ModelInstance *pFrame = GetModelByType(0);
			if (pFrame)
			{
				ModelInstance *pModel = pFrame->GetModelByType(1);
				if (pModel)
				{
					pValue = &GValueFactory->Create(pModel->GetID());
				}
			}
		}
		else if (name == "DoorSone")
		{
			ModelInstance *pFrame = GetModelByType(0);
			if (pFrame)
			{
				ModelInstance *pModel = pFrame->GetModelByType(2);
				if (pModel)
				{
					pValue = &GValueFactory->Create(pModel->GetID());
				}
			}
		}
	}

	return pValue;
}

bool DoorHole::SetFunctionProperty(const std::string &name, const IValue *Value)
{
	if (WallHole::SetFunctionProperty(name, Value))
	{
		return true;
	}

	if (Value)
	{
		std::string ResID = Value->StrValue();

		if (name == "DoorFrame")
		{
			SetModelByType(0, ResID);
			return true;
		}
		else if (name == "Door")
		{
			ModelInstance *pFrame = GetModelByType(0);
			if (pFrame)
			{
				pFrame->SetModelByType(1, ResID);
			}
			return true;
		}
		else if (name == "DoorSone")
		{
			ModelInstance *pFrame = GetModelByType(0);
			if (pFrame)
			{
				pFrame->SetModelByType(2, ResID);
			}
			return true;
		}
	}

	return false;
}

