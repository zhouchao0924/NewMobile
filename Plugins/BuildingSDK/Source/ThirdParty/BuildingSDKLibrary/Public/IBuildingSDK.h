
#pragma once

#include "ISuite.h"
#include "ISerialize.h"
#include "ITransaction.h"
#include "ITargetPlatform.h"

class  IBuildingSDK
{
public:
	virtual bool Initialize() = 0;
	virtual void UnInitialize() = 0;
	virtual ISuite *CreateSuite() = 0;
	virtual void DestroySuite(ISuite *Suite) = 0;
	virtual ISuite *LoadSuite(const char *Filename) = 0;
	virtual IObject *LoadFile(const char *Filename) = 0;
	virtual ISerialize *CreateFileWriter(const char *Filename) = 0;
	virtual ISerialize *CreateFileReader(const char *Filename) = 0;
	virtual ITransact *GetTransaction() = 0;
	virtual IValueFactory *GetValueFactory() = 0;
	virtual void SetTransaction(ITransact *InTransaction) = 0;
	virtual ITargetPlatform *GetPlatform(ETargetPlatform PlatformType) = 0;
};

IBuildingSDK *GetBuildingSDK();

