
#include "ajdr.h"
#include "ModelFile.h"
#include "ResourceMgr.h"
#include "ModelCommons.h"
#include "CurtainComponent.h"
#include "ModelPartComponent.h"
#include "ModelFileComponent.h"

UCurtainComponent::UCurtainComponent(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, Length(10.0f)
{
}

void UCurtainComponent::SetCurtainInfo(const FString &InResRomebar1, const FString &InResCloth1, const FString &InResRomebar2, const FString &InResCloth2, float InLength, float InDistanceBetweenBar)
{
	Cloth1 = InResCloth1;
	Romebar1 = InResRomebar1;
	Cloth2 = InResCloth2;
	Romebar2 = InResRomebar2;
	DistanceBetweenBar = InDistanceBetweenBar;
	Length = InLength;
	CreateModels();
}

void UCurtainComponent::SetLength(float InLength)
{
	Length = InLength;
}

UModelFile *UCurtainComponent::GetRombarModel(int32 index)
{
	UModelFile *RombarModel = nullptr;
	UResourceMgr *ResMgr = UResourceMgr::GetResourceMgr();
	if (ResMgr)
	{
		FString ResID = index == 0 ? Romebar1 : Romebar2;
		RombarModel = Cast<UModelFile>(ResMgr->FindRes(ResID));
		if (RombarModel)
		{
			RombarModel->ForceLoad();
		}
	}
	return RombarModel;
}

UModelFile *UCurtainComponent::GetClothModel(int32 index)
{
	UModelFile *ClothModel = nullptr;
	UResourceMgr *ResMgr = UResourceMgr::GetResourceMgr();
	if (ResMgr)
	{
		FString &ResID = index == 0 ? Cloth1 : Cloth2;
		ClothModel = Cast<UModelFile>(ResMgr->FindRes(ResID));
		if (ClothModel)
		{
			ClothModel->ForceLoad();
		}
	}
	return ClothModel;
}

void UCurtainComponent::CreateModels()
{
	CreateCloths();
	CreateRomerbars();
	UpdateLayout();
}

void UCurtainComponent::CreateCloths()
{
	DestroyCloth();
	AddClothComponents(GetClothModel(0), LeftClothComponents1, RightClothComponents1);
	AddClothComponents(GetClothModel(1), LeftClothComponents2, RightClothComponents2);
}

void UCurtainComponent::AddClothComponents(UModelFile *ClothModel, TArray<UModelPartComponent *> &LeftClothComponents, TArray<UModelPartComponent *> &RightClothComponents)
{
	if (ClothModel)
	{
		FTransform WorldTransform = GetComponentTransform();
		FVector Forward = WorldTransform.Rotator().Vector();

		FVector Size = ClothModel->GetLocalBounds().GetSize();
		TArray<UModelPartComponent *> *ClothComponents[2] = { &LeftClothComponents, &RightClothComponents };
		for (int32 iCloth = 0; iCloth < 2; ++iCloth)
		{
			TArray<FModel*> &Models = ClothModel->GetSubModels();
			for (int i = 0; i < Models.Num(); ++i)
			{
				UModelPartComponent *Comp = NewObject<UModelPartComponent>(GetOwner());
				if (Comp)
				{
					Comp->RegisterComponentWithWorld(GetWorld());
					Comp->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
					Comp->UpdateModel(TArray<FPlane>(), Forward, Size.X, ClothModel, i);
					ClothComponents[iCloth]->Add(Comp);
				}
			}
		}
	}
}

void UCurtainComponent::CreateRomerbars()
{
	DestroyRomerbar();
	AddRomerbarComponents(GetRombarModel(0), RomerBarComponents1);
	AddRomerbarComponents(GetRombarModel(1), RomerBarComponents2);
}

void UCurtainComponent::AddRomerbarComponents(UModelFile *RomebarModel, TArray<UModelPartComponent *> &RomerBarComponents)
{
	RomerBarComponents.SetNum(uint8(ECurtainPart::ERomerbarMax));
	FTransform WorldTransform = GetComponentTransform();
	FVector Forward = WorldTransform.Rotator().Vector();

	if (RomebarModel)
	{
		int32		modelRod = INDEX_NONE;
		FVector		Size = RomebarModel->GetLocalBounds().GetSize();
		FTransform	RelativeTransform;
		RomerbarBodyLength = 0;
		RomerbarHeadLength = 0;

		TArray<FModel*> &Models = RomebarModel->GetSubModels();
		for (int32 i = 0; i < Models.Num(); ++i)
		{
			FModel *pModel = Models[i];
			if (pModel)
			{
				int iPart = 0;
				if (pModel->ModelName == TEXT("Curtain_Rod"))
				{
					modelRod = i;
					RomerbarBodyLength = pModel->Bounds.GetSize().Y;
				}
				else if (pModel->ModelName == TEXT("Curtain_Rod_Head_L"))
				{
					RomerbarHeadLength += pModel->Bounds.GetSize().Y;
					iPart = (int32)ECurtainPart::ERomebarL;
				}
				else if (pModel->ModelName == TEXT("Curtain_Rod_Head_R"))
				{
					RomerbarHeadLength += pModel->Bounds.GetSize().Y;
					iPart = (int32)ECurtainPart::ERomebarR;
				}

				RomerBarComponents[iPart] = AddPart(RomebarModel, i);
			}
		}

		if (modelRod >= 0)
		{
			RomerBarComponents[int(ECurtainPart::ERomebarM)] = AddPart(RomebarModel, modelRod);
		}
	}

}

void UCurtainComponent::SetCloth(int32 index, const FString &InResCloth)
{
	if((index==0 && !(Cloth1 == InResCloth)) || (index == 1 && !(Cloth2 == InResCloth)))
	{
		if (index == 0)
		{
			Cloth1 = InResCloth;
		}
		else
		{
			Cloth2 = InResCloth;
		}
		CreateCloths();
		UpdateLayout();
	}
}

void UCurtainComponent::SetRomerbar(int32 index, const FString &InResRomerbar)
{
	if ( ((index==1) && !(Romebar1 == InResRomerbar)) || ((index == 2) && !(Romebar2 == InResRomerbar)))
	{
		if (index == 0)
		{
			Romebar1 = InResRomerbar;
		}
		else
		{
			Romebar2 = InResRomerbar;
		}
		CreateRomerbars();
		UpdateLayout();
	}
}

UModelPartComponent *UCurtainComponent::AddPart(UModelFile *ModelFile, int32 iModel)
{
	UModelPartComponent *Comp = nullptr;

	if (ModelFile)
	{
		FTransform WorldTransform = GetComponentTransform();
		FVector Forward = WorldTransform.Rotator().Vector();
		FVector Size = ModelFile->GetLocalBounds().GetSize();

		Comp = NewObject<UModelPartComponent>(GetOwner());
		if (Comp)
		{
			Comp->RegisterComponentWithWorld(GetWorld());
			Comp->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
			Comp->UpdateModel(TArray<FPlane>(), Forward, Size.X, ModelFile, iModel);
		}
	}

	return Comp;
}

void UCurtainComponent::DestroyCloth()
{
	DestroyComponents(LeftClothComponents1);
	DestroyComponents(RightClothComponents1);
	DestroyComponents(LeftClothComponents2);
	DestroyComponents(RightClothComponents2);
}

void UCurtainComponent::DestroyRomerbar()
{
	DestroyComponents(RomerBarComponents1);
	DestroyComponents(RomerBarComponents2);
}

void UCurtainComponent::DestroyComponents(TArray<UModelPartComponent *> &Components)
{
	for (int32 i = 0; i < Components.Num(); ++i)
	{
		UModelPartComponent *Comp = Components[i];
		if (Comp)
		{
			Comp->DestroyComponent(true);
		}
	}
	Components.Empty();
}

void UCurtainComponent::DestroyComponent(bool bPromoteChildren /*= false*/)
{	
	DestroyCloth();
	DestroyRomerbar();
	Super::DestroyComponent(bPromoteChildren);
}

void UCurtainComponent::SetCurtainLength(float InLength)
{
	if (Length != InLength)
	{
		Length = InLength;
		UpdateLayout();
	}
}

void UCurtainComponent::SetDistanceBetweenBar(float InDistance)
{
	if (DistanceBetweenBar != InDistance)
	{
		DistanceBetweenBar = InDistance;
		UpdateLayout();
	}
}

bool UCurtainComponent::IsTwoBar()
{
	return Romebar2.Len() > 0;
}

void UCurtainComponent::UpdateLayout()
{
	UpdateComponentsLayout(FVector::ZeroVector, GetRombarModel(0), GetClothModel(0), LeftClothComponents1, RightClothComponents1, RomerBarComponents1);

	if (IsTwoBar())
	{
		FVector TwobarOffset = FVector(-DistanceBetweenBar, 0, 0);
		UpdateComponentsLayout(TwobarOffset, GetRombarModel(1), GetClothModel(1), LeftClothComponents2, RightClothComponents2, RomerBarComponents2);
	}
}

void UCurtainComponent::UpdateComponentsLayout(const FVector &TwoBarOffset, UModelFile *RomebarModel, UModelFile *ClothModel, TArray<UModelPartComponent *> &LeftClothComponents, TArray<UModelPartComponent *> &RightClothComponents, TArray<UModelPartComponent *> &RomerBarComponents)
{
	if (ClothModel)
	{
		FVector Size = ClothModel->GetLocalBounds().GetSize();
		FTransform RelativeTransform = UModelFileComponent::GetOffset(ClothModel, ECenterAdjustType::TopCenter);

		FTransform ClothTransform[2];
		TArray<UModelPartComponent *> *ClothComponents[2] = { &LeftClothComponents, &RightClothComponents };

		//left
		ClothTransform[0] = RelativeTransform;
		FVector Offset = FVector(0, -Length / 2.0f + Size.Y / 2.0f, 0) + TwoBarOffset;
		ClothTransform[0].AddToTranslation(Offset);

		//right
		ClothTransform[1] = RelativeTransform;
		Offset = FVector(0, Length / 2.0f - Size.Y / 2.0f, 0) + TwoBarOffset;
		ClothTransform[1].AddToTranslation(Offset);
		ClothTransform[1].SetScale3D(FVector(1.0f, -1.0f, 1.0f));

		for (int32 iCloth = 0; iCloth < 2; ++iCloth)
		{
			TArray<UModelPartComponent *> &Components = *(ClothComponents[iCloth]);
			for (int32 iComp = 0; iComp < Components.Num(); ++iComp)
			{
				UModelPartComponent *Comp = Components[iComp];
				if (Comp)
				{
					Comp->SetRelativeTransform(ClothTransform[iCloth]);
				}
			}
		}
	}

	if (RomebarModel)
	{
		FTransform RelativeTransform;
		int32 RomerbarL = (int32)ECurtainPart::ERomebarL;
		int32 RomerbarR = (int32)ECurtainPart::ERomebarR;
		int32 RomerbarM = (int32)ECurtainPart::ERomebarM;

		if (RomerBarComponents.IsValidIndex(RomerbarL))
		{
			UModelPartComponent *Comp = RomerBarComponents[RomerbarL];
			if (Comp)
			{
				FModel *SubModel = Comp->GetModel();
				RelativeTransform = UModelFileComponent::GetOffset(SubModel->Bounds, FVector(1.0f), ECenterAdjustType::LeftCenter);
				FVector Offset = RelativeTransform.GetLocation();
				Offset = FVector(0, Offset.Y - Length / 2.0f, 0) + TwoBarOffset;
				RelativeTransform.SetLocation(Offset);
				Comp->SetRelativeTransform(RelativeTransform);
			}
		}

		if (RomerBarComponents.IsValidIndex(RomerbarR))
		{
			UModelPartComponent *Comp = RomerBarComponents[RomerbarR];
			if (Comp)
			{
				FModel *SubModel = Comp->GetModel();
				RelativeTransform = UModelFileComponent::GetOffset(SubModel->Bounds, FVector(1.0f), ECenterAdjustType::RightCenter);
				FVector Offset = RelativeTransform.GetLocation();
				Offset = FVector(0, Offset.Y + Length / 2.0f, 0) + TwoBarOffset;
				RelativeTransform.SetLocation(Offset);
				Comp->SetRelativeTransform(RelativeTransform);
			}
		}

		if (RomerBarComponents.IsValidIndex(RomerbarM))
		{
			UModelPartComponent *Comp = RomerBarComponents[RomerbarM];
			if (Comp)
			{
				float DesireLength = Length;
				RelativeTransform = UModelFileComponent::GetOffset(RomebarModel, ECenterAdjustType::TopCenter);
				FVector Offset = RelativeTransform.GetLocation();
				Offset = FVector(0, Offset.Y, 0) + TwoBarOffset;
				RelativeTransform.SetLocation(Offset);
				RelativeTransform.SetScale3D(FVector(1.0f, DesireLength / RomerbarBodyLength, 1.0f));
				Comp->SetRelativeTransform(RelativeTransform);
			}
		}
	}
}


