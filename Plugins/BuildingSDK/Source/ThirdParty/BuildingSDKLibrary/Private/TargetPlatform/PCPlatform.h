
#pragma once

#include "ITargetPlatform.h"

class PCPlatform :public ITargetPlatform
{
public:
	bool Cook(IObject *Object, const char *OutputFilename) override;
	ETargetPlatform GetPlatformType() override { return ETargetPC; }
};


