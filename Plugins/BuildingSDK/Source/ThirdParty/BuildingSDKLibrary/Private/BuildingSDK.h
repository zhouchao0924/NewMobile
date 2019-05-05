
#pragma once

#include "Platform.h"
#include "IBuildingSDK.h"
#include "ITargetPlatform.h"
#include "Transaction/Transaction.h"
#include "Elements/SuiteImpl.h"
#include "Class/ValueFactory.h"

class BuildingSDKImpl :public IBuildingSDK
{
public:
	BuildingSDKImpl();
	bool Initialize() override;
	void UnInitialize() override;
	ISuite *CreateSuite() override;
	void DestroySuite(ISuite *Suite) override;
	ITransact *GetTransaction() override;
	ISuite *LoadSuite(const char *Filename) override;
	ISerialize *CreateFileWriter(const char *Filename) override;
	ISerialize *CreateFileReader(const char *Filename) override;
	void SetTransaction(ITransact *InTransaction) override;
	IObject *LoadFile(const char *Filename) override;
	IValueFactory *GetValueFactory() override;
	ITargetPlatform *GetPlatform(ETargetPlatform PlatformType) override;
protected:
	ITransact						*_Transaction;
	ValueFactory					_ValueFactoy;
	bool							_bInitialized;
	std::vector<SuiteImpl *>		_Suites;
	std::vector<ITargetPlatform *>	_Platforms;
};


