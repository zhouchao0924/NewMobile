
#pragma once

#include "IClass.h"

enum ETargetPlatform
{
	ETargetPC,
	ETargetIOS,
	ETargetWebGL,
	ETargetAndroid
};

class ITargetPlatform
{
public:
	virtual ~ITargetPlatform() { }
	virtual bool Cook(IObject *Object, const char *OutputFilename) = 0;
	virtual ETargetPlatform GetPlatformType() = 0;
};


