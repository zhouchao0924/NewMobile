// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDR, Log, All);

#define  FORCE_TYPE(type, v)  (*((type*)&(v)))
#define  FORCE_POINTER(type, v)  ((type)(v))

#pragma warning(disable : 4668)