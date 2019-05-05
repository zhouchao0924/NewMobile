
#pragma once

#include "ITargetPlatform.h"

class WebGLPlatform :public ITargetPlatform
{
public:
	bool Cook(IObject *Object, const char *OutputFilename) override;
	ETargetPlatform GetPlatformType() override { return ETargetWebGL; }
};


