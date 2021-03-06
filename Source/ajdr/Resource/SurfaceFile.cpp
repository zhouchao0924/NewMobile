﻿
#include "ajdr.h"
#include "SurfaceFile.h"
#include "DRGameMode.h"
#include "ResourceUtils.h"
//#include "ModelCompress.h"
//#include "ModelImporter.h"
#include "ResourceMgr.h"

void USurfaceFile::FHeader::Serialize(FArchive &Ar)
{
	FResourceSummary::Serialize(Ar);

	if (Ar.IsSaving())
	{
		int32 NumTex = Textures.Num();
		Ar << NumTex;
		for (int32 i = 0; i < NumTex; ++i)
		{
			Textures[i].Serialize(Ar, BodyVersion);
		}

		Ar << Image.SizeX;
		Ar << Image.SizeY;

		int32 NumBytes = Image.Data.Num();
		Ar << NumBytes;
		if (NumBytes > 0)
		{
			Ar.Serialize(Image.Data.GetData(), NumBytes);
		}
	}
	else if (Ar.IsLoading())
	{
		int32 NumTex = 0;
		Ar << NumTex;
		Textures.SetNum(NumTex);
		for (int32 i = 0; i < NumTex; ++i)
		{
			Textures[i].Serialize(Ar, BodyVersion);
		}
		
		if (HeadVersion > 3)
		{
			if (HeadVersion > 4)
			{
				Ar << Image.SizeX;
				Ar << Image.SizeY;
			}

			int32 NumBytes = 0;
			Ar << NumBytes;
			Image.Data.SetNum(NumBytes);
			if (NumBytes > 0)
			{
				Ar.Serialize(Image.Data.GetData(), NumBytes);
			}
		}
	}

}

USurfaceFile::USurfaceFile(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
	,m_MaterialInfo(NULL)
{
}

int32 USurfaceFile::GetNumerOfTexureSlot()
{
	TArray<FTexSlotInfo> TexSlots;
	m_MaterialInfo->GetRefTextures(TexSlots, true);
	return TexSlots.Num();
}

UTexture2D * USurfaceFile::GetTexureSlotInfo(int32 iTexSlot, FString & TexSlotName, FString &TexName)
{
	UTexture2D *Tex = NULL;
	TArray<FTexSlotInfo> TexSlots;
	m_MaterialInfo->GetRefTextures(TexSlots, true);

	FTexSlotInfo &Slot = TexSlots[iTexSlot];
	TexSlotName = Slot.TexParamName.ToString();
	TexName = Slot.TexValue->GetName();
	int32 Value = m_MaterialInfo->GetTextureValue(Slot.TexParamName);

	if (Value != INDEX_NONE)
	{
		Tex = Cast<UTexture2D>(m_Textures[Value]->GetTexture());
	}
	else
	{
		Tex = Cast<UTexture2D>(Slot.TexValue);
	}

	return Tex;
}

UTexture2D * USurfaceFile::SetMaterialSlotTexture(int32 TextureIndex, const FString & NewTexFile)
{
	UTexture2D *Texture = NULL;
	TArray<FTexSlotInfo> texSlots;
	m_MaterialInfo->GetRefTextures(texSlots, true);

	if (texSlots.IsValidIndex(TextureIndex))
	{
		FTexSlotInfo &slotInfo = texSlots[TextureIndex];

		/*UTextureImporter *TexImporter = ResMgr->GetTextureImporter();

		if (TexImporter)
		{
			FTextureSourceInfo *pTexSourceInfo = TexImporter->ImportFromFile(this, GetSlotType(slotInfo.TexParamName), NewTexFile);
			if (pTexSourceInfo)
			{
				FModelTexture *tex = NULL;

				int32 TexIndex = m_MaterialInfo->GetTextureValue(slotInfo.TexParamName);
				if (TexIndex != INDEX_NONE)
				{
					tex = m_Textures[TexIndex];
				}
				else
				{
					TexIndex = m_Textures.Num();
					tex = new FModelTexture();
					m_Textures.Add(tex);
					m_MaterialInfo->SetTextureValue(slotInfo.TexParamName, TexIndex);
				}

				tex->SetData(*pTexSourceInfo);
				Texture = tex->GetTexture();
				MarkDirty();
			}
		}*/
	}
	return Texture;
}

UTexture2D * USurfaceFile::GetSlotNTexture(int32 TextureIndex, const FString & NewTexFile)
{

	UTexture2D *Texture = NULL;
	TArray<FTexSlotInfo> texSlots;
	m_MaterialInfo->GetRefTextures(texSlots, true);

	if (texSlots.IsValidIndex(TextureIndex))
	{
		FTexSlotInfo &slotInfo = texSlots[TextureIndex];

		/*UTextureImporter *TexImporter = ResMgr->GetTextureImporter();

		if (TexImporter)
		{
			FTextureSourceInfo *pTexSourceInfo = TexImporter->ImportFromFile(this, GetSlotType(slotInfo.TexParamName), NewTexFile);
			if (pTexSourceInfo)
			{
				FModelTexture *tex =  new FModelTexture();
				tex->SetData(*pTexSourceInfo);
				Texture = tex->GetTexture();

			}
		}*/
	}
	return Texture;
}

int32 USurfaceFile::GetNumberOfScalarValues()
{
	TArray<FScalarSlotInfo> ValueSlots;
	m_MaterialInfo->GetRefValues(ValueSlots, true);
	return ValueSlots.Num();
}

float USurfaceFile::GetScalarValue(int32 iSlot, FString & Name)
{
	TArray<FScalarSlotInfo> ValueSlots;
	m_MaterialInfo->GetRefValues(ValueSlots, true);
	if (ValueSlots.IsValidIndex(iSlot))
	{
		FScalarSlotInfo &Slot = ValueSlots[iSlot];
		Name = Slot.ParamName.ToString();

		int32 FoundIndex = m_MaterialInfo->FindFloatIndex(Slot.ParamName);
		if (FoundIndex != INDEX_NONE)
		{
			return m_MaterialInfo->GetFloatValue(Slot.ParamName);
		}
		return Slot.FloatValue;
	}
	return 0.0f;
}

void USurfaceFile::SetScalarValue(int32 iSlot, float Value)
{
	TArray<FScalarSlotInfo> ValueSlots;
	m_MaterialInfo->GetRefValues(ValueSlots, true);
	if (ValueSlots.IsValidIndex(iSlot))
	{
		FScalarSlotInfo &Slot = ValueSlots[iSlot];
		if (Slot.FloatValue != Value)
		{
			m_MaterialInfo->SetFloatValue(Slot.ParamName, Value);
			MarkDirty();
		}
	}
}

int32 USurfaceFile::GetNumberOfVectorValues()
{
	TArray<FVectorSlotInfo> ValueSlots;
	m_MaterialInfo->GetRefValues(ValueSlots, true);
	return ValueSlots.Num();
}

FLinearColor USurfaceFile::GetVectorValue(int32 iSlot, FString & Name)
{
	TArray<FVectorSlotInfo> ValueSlots;
	m_MaterialInfo->GetRefValues(ValueSlots, true);
	if (ValueSlots.IsValidIndex(iSlot))
	{
		FVectorSlotInfo &Slot = ValueSlots[iSlot];
		Name = Slot.ParamName.ToString();

		int32 FoundIndex = m_MaterialInfo->FindVectorIndex(Slot.ParamName);
		if (FoundIndex != INDEX_NONE)
		{
			return m_MaterialInfo->GetVectorValue(Slot.ParamName);
		}
		return Slot.LinearValue;
	}
	return FLinearColor::Black;
}

void USurfaceFile::SetVectorValue(int32 iSlot, const FLinearColor & Value)
{
	TArray<FVectorSlotInfo> ValueSlots;
	m_MaterialInfo->GetRefValues(ValueSlots, true);
	if (ValueSlots.IsValidIndex(iSlot))
	{
		FVectorSlotInfo &Slot = ValueSlots[iSlot];
		if (Slot.LinearValue != Value)
		{
			m_MaterialInfo->SetVectorValue(Slot.ParamName, Value);
			MarkDirty();
		}
	}
}

UTexture * USurfaceFile::GetMaterialPreviewImage(int32 SizeX, int32 SizeY)
{
	UTexture *MtrlPreviewImage = NULL;

	
		FModelMaterial *mtrl = GetMaterial();
		if (mtrl)
		{
			UMaterialInterface *ue4Mtrl = mtrl->GetUE4Material();
			//增加根据材质类型获取名字
			if (mtrl->GetType() == EMaterialRef)
			{
				if (!ue4Mtrl)
				{
					FModelMaterialRef *MaterialRef = (FModelMaterialRef *)mtrl;
					USurfaceFile *Surface = Cast<USurfaceFile>(MaterialRef->RefSurface.Get());
					UResource *re = Surface->ResMgr->FindRes(Surface->GetResID(), false);
					re->ForceLoad();
					ue4Mtrl = Surface->GetUE4Material();
				}

			}

			if (ue4Mtrl)
			{
				UMaterialInstanceDynamic *dynMtrl = UMaterialInstanceDynamic::Create(ue4Mtrl, NULL);
				if (dynMtrl)
				{
					mtrl->UpdateParameters(dynMtrl, m_Textures);
					MtrlPreviewImage = UResourceUtils::GetMaterialPreviewTexture(this, SizeX, SizeY, dynMtrl);
				}
			}
		}
	return MtrlPreviewImage;
}

UMaterialInstanceDynamic * USurfaceFile::GetDynamicMaterialInstance()
{
	UMaterialInstanceDynamic *DynamicMtrl = NULL;

	FModelMaterial *material = GetMaterial();
	if (material)
	{
		UMaterialInterface *ue4Mtrl = material->GetUE4Material();
		if (ue4Mtrl)
		{
			DynamicMtrl = UMaterialInstanceDynamic::Create(ue4Mtrl, this);
			material->UpdateParameters(DynamicMtrl, m_Textures);
		}
	}

	return DynamicMtrl;
}

void USurfaceFile::BeginDestroy()
{
	Clean();
	Super::BeginDestroy();
}

void USurfaceFile::SerializeHeader(FArchive &Ar)
{
	Super::SerializeHeader(Ar);

	if (Ar.IsSaving())
	{
		uint32 type = m_MaterialInfo ? m_MaterialInfo->GetType() : EMaterialNone;
		Ar << type;
		if (m_MaterialInfo)
		{
			m_MaterialInfo->Serialize(Ar, m_Header.BodyVersion);
		}
	}
	else if(Ar.IsLoading())
	{
		uint32 type = 0;
		Ar << type;

		m_MaterialInfo = new FModelMaterialUE4();
		if (m_MaterialInfo)
		{
			m_MaterialInfo->Serialize(Ar, m_Header.BodyVersion);
		}
	}
	
}

void  USurfaceFile::Serialize(FArchive &Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsSaving())
	{
		int64 HeaderPos = Ar.Tell();

		int32 NumTextures = m_Textures.Num();
		m_Header.Textures.SetNum(NumTextures);

		SerializeHeader(Ar);

		//serialze texture
		for (int32 iTexture = 0; iTexture < m_Header.Textures.Num(); ++iTexture)
		{
			FChunk &texChunk = m_Header.Textures[iTexture];
			texChunk.Offset = (int32)Ar.Tell();
			m_Textures[iTexture]->Serialize(Ar, m_Header.BodyVersion);
			texChunk.Size = int32(Ar.Tell() - texChunk.Offset);
		}

		int64 TailPos = Ar.Tell();
		Ar.Seek(HeaderPos);

		SerializeHeader(Ar);

		Ar.Seek(TailPos);
	}
	else if (Ar.IsLoading())
	{
		SerializeHeader(Ar);

		//serialze texture
		for (int32 iTexture = 0; iTexture < m_Header.Textures.Num(); ++iTexture)
		{
			FChunk texChunk = m_Header.Textures[iTexture];
			Ar.Seek(texChunk.Offset);
			FModelTexture *newTexutre = new FModelTexture();
			newTexutre->Serialize(Ar, m_Header.BodyVersion);
			m_Textures.Add(newTexutre);
		}
	}
	
}

void USurfaceFile::Unload()
{
	Clean();
	Super::Unload();
}

void USurfaceFile::SerizlizeHistory(FArchive &Ar, uint32 HistoryFileVer)
{
	//serialize surface
}

void USurfaceFile::Clean()
{
	for (int32 i = 0; i < m_Textures.Num(); ++i)
	{
		SAFE_DELETE(m_Textures[i]);
	}

	m_Textures.Empty();
}

FModelMaterial * USurfaceFile::GetMaterial()
{
	return m_MaterialInfo;
}

UMaterialInterface *USurfaceFile::GetUE4Material()
{
	if (!m_MaterialInfo)
	{
		ForceLoad();
		
	}
	return m_MaterialInfo->GetUE4Material();
}

void USurfaceFile::BuildPreviewImage()
{
	UMaterialInstanceDynamic *DynMtrl = NULL;
	UMaterialInterface *Material = GetUE4Material();

	if (IsNeedLoad())
	{
		ForceLoad();
	}

	if (Material)
	{
		DynMtrl = UMaterialInstanceDynamic::Create(Material, this);
		if (DynMtrl)
		{
			check(m_MaterialInfo->GetType() == EMaterialUE4)
			UpdateParameters(DynMtrl);
		}
	}

	if (DynMtrl)
	{
		int32 SizeX = 0, SizeY = 0;
		UResourceUtils::GetPreviewMaterialImageSize(SizeX, SizeY);

		UTextureRenderTarget2D *TexTarget = UResourceUtils::GetMaterialPreviewTexture(this, SizeX, SizeY, DynMtrl);
		if (TexTarget)
		{
			UResourceUtils::SaveImage(this, TexTarget);
		}
	}
}

void USurfaceFile::UpdateParameters(UMaterialInstanceDynamic *Mtrl)
{
	if (Mtrl && m_MaterialInfo)
	{
		m_MaterialInfo->UpdateParameters(Mtrl, m_Textures);
	}
}

void USurfaceFile::CopyOverrideParameters(FModelMaterial *Material)
{
	if (m_MaterialInfo)
	{
		m_MaterialInfo->CopyOverrideParametersTo(Material);
	}
}

FModelMaterialUE4 *USurfaceFile::GetMaterialUE4()
{
	if (m_MaterialInfo && m_MaterialInfo->GetType() == EMaterialUE4)
	{
		return (FModelMaterialUE4 *)m_MaterialInfo;
	}
	return NULL;
}

void USurfaceFile::SetType(EMaterialType InType)
{
	if (m_MaterialInfo != NULL && m_MaterialInfo->GetType()!=InType)
	{
		SAFE_DELETE(m_MaterialInfo);
	}

	if (!m_MaterialInfo)
	{
		if (InType == EMaterialUE4)
		{
			m_MaterialInfo = new FModelMaterialUE4();
		}
	}
}

void USurfaceFile::SetSurface(FModelMaterial *Material, TArray<FModelTexture *> &Textures)
{
	if (Material->GetType() == EMaterialRef)
	{
		Clean();

		FModelMaterialUE4 *newMtrlUE4 = NULL;
		FModelMaterialRef *mtrlUE4 = (FModelMaterialRef *)Material;
		
		USurfaceFile *Surface = Cast<USurfaceFile>(mtrlUE4->RefSurface.Get());
		if (Surface && Surface->m_MaterialInfo)
		{
			newMtrlUE4 = new FModelMaterialUE4();
			newMtrlUE4->Name = Surface->m_MaterialInfo->Name;
			newMtrlUE4->UE4Path = Surface->m_MaterialInfo->UE4Path;

			for (int32 i = 0; i < Surface->m_MaterialInfo->OverrideTextureParameters.Num(); ++i)
			{
				FSurfaceParameterTexture &SurfTexParam = Surface->m_MaterialInfo->OverrideTextureParameters[i];
				int32 FoundIndex = newMtrlUE4->FindTextureIndex(SurfTexParam.ParamName);
				if (FoundIndex == INDEX_NONE)
				{
					int32 Value = m_Textures.Num();
					newMtrlUE4->SetTextureValue(SurfTexParam.ParamName, Value);

					FModelTexture *surfTex = Surface->m_Textures[SurfTexParam.Value];
					FModelTexture *newTex = new FModelTexture();
					newTex->CopyFrom(*surfTex);
					m_Textures.Add(newTex);
				}
			}
		}

		for (int32 i = 0; i < Material->OverrideTextureParameters.Num(); ++i)
		{
			FSurfaceParameterTexture &TexParam = Material->OverrideTextureParameters[i];

			if (Textures.IsValidIndex(TexParam.Value))
			{
				FModelTexture *tex = Textures[TexParam.Value];
				check(tex != nullptr);

				FModelTexture *newTex = new FModelTexture();
				newTex->CopyFrom(*tex);

				int32 Value = m_Textures.Num();
				newMtrlUE4->SetTextureValue(TexParam.ParamName, Value);

				m_Textures.Add(newTex);
			}
		}

		Material->CopyOverrideParametersTo(newMtrlUE4);

		SAFE_DELETE(Material);
		m_MaterialInfo = newMtrlUE4;
	}
	else
	{
		Clean();

		for (int32 i = 0; i < Material->OverrideTextureParameters.Num(); ++i)
		{
			FSurfaceParameterTexture &TexParam = Material->OverrideTextureParameters[i];
			
			if (TexParam.Value != INDEX_NONE)
			{
				FModelTexture *tex = Textures[TexParam.Value];
				check(tex != nullptr);

				FModelTexture *newTex = new FModelTexture();
				newTex->CopyFrom(*tex);

				TexParam.Value = m_Textures.Num();;
				m_Textures.Add(newTex);
			}
		}

		m_MaterialInfo = (FModelMaterialUE4 *)Material;
	}
}

void USurfaceFile::CopyOverrideTextures(FModelMaterial *Material, TArray<FModelTexture *> &Textures)
{
	for (int32 i = 0; i < Material->OverrideTextureParameters.Num(); ++i)
	{
		FSurfaceParameterTexture &TexParam = Material->OverrideTextureParameters[i];

		if (TexParam.Value != INDEX_NONE)
		{
			FModelTexture *tex = Textures[TexParam.Value];
			check(tex != nullptr);

			FModelTexture *newTex = new FModelTexture();
			newTex->CopyFrom(*tex);

			TexParam.Value = m_Textures.Num();;
			m_Textures.Add(newTex);
		}
	}
}

UTexture *USurfaceFile::GetTexture(int32 iSlot)
{
	UTexture *Tex = NULL;

	TArray<FTexSlotInfo> TexSlots;
	m_MaterialInfo->GetRefTextures(TexSlots, true);

	FTexSlotInfo &Slot = TexSlots[iSlot];
	int32 Value = m_MaterialInfo->GetTextureValue(Slot.TexParamName);

	if (Value != INDEX_NONE)
	{
		Tex = m_Textures[Value]->GetTexture();
	}
	else
	{
		Tex = Slot.TexValue;
	}

	return Tex;
}

FModelTexture *USurfaceFile::GetTextureByName(const FName &Name)
{
	int32 TexValue = m_MaterialInfo->GetTextureValue(Name);

	if (m_Textures.IsValidIndex(TexValue))
	{
		return m_Textures[TexValue];
	}

	return NULL;
}

void USurfaceFile::CheckResource()
{
	for (int32 i = 0; i < m_Textures.Num(); ++i)
	{
		FModelTexture *tex = m_Textures[i];
		/*if (tex && tex->Source.CompressedImages.Num() == 0)
		{
			CompressUtil::CompressTexture(tex);
		}*/
	}
	Super::CheckResource();
}

