
#pragma once

#include "ITargetPlatform.h"

class AndroidPlatform :public ITargetPlatform
{
public:
	bool Cook(IObject *Object, const char *OutputFilename) override;
	ETargetPlatform GetPlatformType() override { return ETargetAndroid; }
};


