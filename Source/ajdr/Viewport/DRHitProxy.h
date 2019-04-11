// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

struct HBuildingObject : public HHitProxy
{
	DECLARE_HIT_PROXY();
	
	uint32 BuildingID;
	
	HBuildingObject(uint32 InBuildingID)
		: HHitProxy(HPP_UI)
		, BuildingID(InBuildingID)
	{
	}

	void AddReferencedObjects(FReferenceCollector& Collector) override
	{
	}
};

