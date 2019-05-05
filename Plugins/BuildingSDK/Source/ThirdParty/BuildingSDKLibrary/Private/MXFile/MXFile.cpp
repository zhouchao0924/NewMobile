
#include "assert.h"
#include "MXFile.h"
#include "MXTexture.h"
#include "IBuildingSDK.h"
#include "Mesh/MeshObject.h"
#include "Stream/FileStream.h"
#include "Mesh/SurfaceObject.h"
#include "Math/kString.h"

static kString FingerMark = "1*&343!2)*Yds;Qd^`}/?3,";

bool LoadBool(ISerialize &Ar)
{
	int Value = 0;
	Ar << Value;
	return Value != 0;
}

void SaveBool(ISerialize &Ar, bool bValue)
{
	int Value = bValue ? 1 : 0;
	Ar << Value;
}

kBox3D LoadBox(ISerialize &Ar)
{
	kBox3D Value;
	unsigned char bValid;
	Ar << Value;
	Ar << bValid;
	return Value;
}

void SaveBox(ISerialize &Ar, kBox3D &box)
{
	unsigned char bValid = box.IsInvalid()? 0 : 1;
	Ar << box;
	Ar << bValid;
}

void FChunk::Serialize(ISerialize &Ar, int Ver)
{
	Ar << Offset;
	Ar << Size;
}

FResourceSummary::FResourceSummary()
	: CompressedFlag(0)
	, HeadVersion(-1)
	, ResType(EResUnknown)
	, ModifyVersion(0)
{
}

void  FResourceSummary::Serialize(ISerialize &Ar)
{
	HeadVersion = GetHeadVer();
	BodyVersion = GetCodeVer();

	if (Ar.IsLoading())
	{
		Ar << HeadVersion;
		Ar << ResID;
		Ar << LocalVersion;

		Ar << CompressedFlag;
		Ar << ResourceName;

		//ver2 <--
		if (HeadVersion <= RESOURCE_HEADERVER_2)
		{
			int	SizeX, SizeY, SizeZ;
			kString Brand, Subfamily, CategoryName, Desc;
			char	PerfPosition;
			bool	bUsePhysics;
			Ar << Brand;
			Ar << Subfamily;
			Ar << CategoryName;
			Ar << PerfPosition;
			bUsePhysics = LoadBool(Ar);
			Ar << SizeX;
			Ar << SizeY;
			Ar << SizeZ;
			Ar << Desc;
		}

		if (HeadVersion > RESOURCE_HEADERVER_2)
		{
			Ar << ModifyVersion;
		}

		int NumDeps = 0;
		Ar << NumDeps;
		Dependences.resize(NumDeps);
		for (int i = 0; i < Dependences.size(); ++i)
		{
			Ar << Dependences[i];
		}

		Ar << BodyVersion;
	}
	else if (Ar.IsSaving())
	{
		Ar << HeadVersion;
		Ar << ResID;
		Ar << LocalVersion;

		Ar << CompressedFlag;
		Ar << ResourceName;
		
		Ar << ModifyVersion;

		int NumDeps = Dependences.size();
		Ar << NumDeps;
		for (int i = 0; i < Dependences.size(); ++i)
		{
			Ar << Dependences[i];
		}

		Ar << BodyVersion;
	}
}

MXFile::MXFile()
	:Scale3D(1.0f)
{
}

BEGIN_DERIVED_CLASS(MXFile, Object)
END_CLASS()

void MXFile::FHeader::Serialize(ISerialize &Ar)
{
	FResourceSummary::Serialize(Ar);

	size_t pos0 = Ar.Tell();

	if (Ar.IsSaving())
	{
		Ar << Image.SizeX;
		Ar << Image.SizeY;

		int NumBytes = Image.Data.size();
		Ar << NumBytes;
		if (NumBytes > 0)
		{
			Ar.Serialize(&Image.Data[0], NumBytes);
		}

		int NumModel = Models.size();
		Ar << NumModel;
		for (int i = 0; i < NumModel; ++i)
		{
			Models[i].Serialize(Ar, BodyVersion);
		}

		int NumMtrl = Materials.size();
		Ar << NumMtrl;
		for (int i = 0; i < NumMtrl; ++i)
		{
			Materials[i].Serialize(Ar, BodyVersion);
		}

		int NumTex = Textures.size();
		Ar << NumTex;
		for (int i = 0; i < NumTex; ++i)
		{
			Textures[i].Serialize(Ar, BodyVersion);
		}
	}
	else if (Ar.IsLoading())
	{
		Ar << Image.SizeX;
		Ar << Image.SizeY;

		int NumBytes = 0;
		Ar << NumBytes;
		Image.Data.resize(NumBytes);
		if (NumBytes > 0)
		{
			Ar.Serialize(&Image.Data[0], NumBytes);
		}

		int NumModel = 0;
		Ar << NumModel;
		Models.resize(NumModel);
		for (int i = 0; i < NumModel; ++i)
		{
			Models[i].Serialize(Ar, BodyVersion);
		}

		int NumMtrl = 0;
		Ar << NumMtrl;
		Materials.resize(NumMtrl);
		for (int i = 0; i < NumMtrl; ++i)
		{
			Materials[i].Serialize(Ar, BodyVersion);
		}

		int NumTex = 0;
		Ar << NumTex;
		Textures.resize(NumTex);
		for (int i = 0; i < NumTex; ++i)
		{
			Textures[i].Serialize(Ar, BodyVersion);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void SaveMark(char resType, ISerialize &Ar)
{
	if (Ar.IsSaving())
	{
		Ar << resType;
		Ar << FingerMark;
	}
}

bool CheckMark(ISerialize &Ar)
{
	if (Ar.IsLoading())
	{
		kString Mark;
		Ar << Mark;
		if (Mark == FingerMark)
		{
			return true;
		}
	}
	return false;
}

bool SkipMark(ISerialize &Ar)
{
	if (Ar.IsLoading())
	{
		char resType = 0;
		Ar << resType;
		return CheckMark(Ar);
	}
	return false;
}

MXFile *MXFile::LoadFromFile(const char *InFilename)
{
	MXFile *mxFile = NULL;

	ISerialize *Reader = GetBuildingSDK()->CreateFileReader(InFilename);
	if (Reader)
	{
		if (SkipMark(*Reader))
		{
			mxFile = new MXFile();
			mxFile->Filename = InFilename;
			mxFile->Serialize(*Reader);
		}
		Reader->Close();
	}

	return mxFile;
}

void MXFile::ResetSize()
{
	kVector3D Size = LocalBounds.GetExtent()*Scale3D*10.0f;
	DepthInMM = std::round(Size.X);
	WidthInMM = std::round(Size.Y);
	HeightInMM = std::round(Size.Z);
}

void MXFile::CaclScale3D()
{
	kBox3D Bounds = LocalBounds;
	kVector3D Size = Bounds.GetExtent();
	Scale3D.X = DepthInMM *0.1f / Size.X;
	Scale3D.Y = WidthInMM *0.1f / Size.Y;
	Scale3D.Z = HeightInMM*0.1f / Size.Z;
}

ISurfaceObject *MXFile::GetMeshSurface(int Index)
{
	if (Index >= 0 && Index < m_Materials.size())
	{
		return m_Materials[Index];
	}
	return nullptr;
}

void MXFile::Link(int SurfaceID, int SubModelIndex)
{
}

void MXFile::UnLink(int SurfaceID, int SubModelIndex)
{
}

void MXFile::Serialize(ISerialize &Ar)
{
	if (Ar.IsSaving())
	{
		int HeaderPos = Ar.Tell();

		int NumModels = Meshes.size();
		m_Header.Models.resize(NumModels);

		int NumMaterials = m_Materials.size();
		m_Header.Materials.resize(NumMaterials);

		int NumTextures = m_Textures.size();
		m_Header.Textures.resize(NumTextures);

		SerializeHeader(Ar);

		//serialize models
		for (int iModel = 0; iModel < NumModels; ++iModel)
		{
			FChunk &modelChunk = m_Header.Models[iModel];
			modelChunk.Offset = (int)Ar.Tell();
			Meshes[iModel]->Serialize(Ar, m_Header.BodyVersion);
			modelChunk.Size = int(Ar.Tell() - modelChunk.Offset);
		}

		//serialize materials
		for (int iMaterial = 0; iMaterial < NumMaterials; ++iMaterial)
		{
			FChunk &mtrlChunk = m_Header.Materials[iMaterial];
			mtrlChunk.Offset = (int)Ar.Tell();

			int type = (int)m_Materials[iMaterial]->GetType();
			Ar << type;

			m_Materials[iMaterial]->Serialize(Ar, m_Header.BodyVersion);
			mtrlChunk.Size = int(Ar.Tell() - mtrlChunk.Offset);
		}

		//serialze texture
		for (int iTexture = 0; iTexture < m_Header.Textures.size(); ++iTexture)
		{
			FChunk &texChunk = m_Header.Textures[iTexture];
			texChunk.Offset = (int)Ar.Tell();
			m_Textures[iTexture]->Serialize(Ar, m_Header.BodyVersion);
			texChunk.Size = int(Ar.Tell() - texChunk.Offset);
		}

		SaveBox(Ar, LocalBounds);
		Ar << ViewLoc.EyeLoc;
		Ar << ViewLoc.FocusLoc;
		Ar << ViewLoc.EyeDistance;

		if (m_Header.BodyVersion > MODELFILE_BODY_VER_3)
		{
			KSERIALIZE_ENUM(ECenterAdjustType, CenterType);
			Ar << Offset;
		}

		if (m_Header.BodyVersion == MODELFILE_BODY_VER_5)
		{
			Ar << Scale3D;
			ResetSize();
		}
		else if (m_Header.BodyVersion > MODELFILE_BODY_VER_5)
		{
			Ar << DepthInMM;
			Ar << WidthInMM;
			Ar << HeightInMM;
			CaclScale3D();
		}
		else
		{
			ResetSize();
		}

		int TailPos = Ar.Tell();
		Ar.Seek(HeaderPos);

		SerializeHeader(Ar);

		Ar.Seek(TailPos);
	}
	else if (Ar.IsLoading())
	{
		SerializeHeader(Ar);

		//serialize models
		Meshes.resize(m_Header.Models.size());
		for (int iModel = 0; iModel < m_Header.Models.size(); ++iModel)
		{
			FChunk modelChunk = m_Header.Models[iModel];
			Ar.Seek(modelChunk.Offset);

			MeshObject *meshObj = new MeshObject(nullptr);
			meshObj->SetMeshObjectCallback(this);
			meshObj->Serialize(Ar, m_Header.BodyVersion);
			Meshes[iModel] = meshObj;
		}

		//serialize materials
		for (int iMaterial = 0; iMaterial < m_Header.Materials.size(); ++iMaterial)
		{
			FChunk mtrlChunk = m_Header.Materials[iMaterial];
			Ar.Seek(mtrlChunk.Offset);

			int type = 0;
			Ar << type;

			SurfaceObject *pSurface = new SurfaceObject();
			pSurface->SetType(type);
			pSurface->Serialize(Ar, m_Header.BodyVersion);
			m_Materials.push_back(pSurface);
		}

		//serialze texture
		for (int iTexture = 0; iTexture < m_Header.Textures.size(); ++iTexture)
		{
			FChunk texChunk = m_Header.Textures[iTexture];
			Ar.Seek(texChunk.Offset);
			FModelTexture *newTexutre = new FModelTexture();
			newTexutre->Serialize(Ar, m_Header.BodyVersion);
			m_Textures.push_back(newTexutre);
		}

		LocalBounds = LoadBox(Ar);
		Ar << ViewLoc.EyeLoc;
		Ar << ViewLoc.FocusLoc;
		Ar << ViewLoc.EyeDistance;

		if (m_Header.BodyVersion > MODELFILE_BODY_VER_3)
		{
			KSERIALIZE_ENUM(ECenterAdjustType, CenterType);
			Ar << Offset;
		}

		if (m_Header.BodyVersion == MODELFILE_BODY_VER_5)
		{
			Ar << Scale3D;
			ResetSize();
		}
		else if (m_Header.BodyVersion > MODELFILE_BODY_VER_5)
		{
			size_t pos = Ar.Tell();
			Ar << DepthInMM;
			Ar << WidthInMM;
			Ar << HeightInMM;
			CaclScale3D();
		}
		else
		{
			ResetSize();
		}
	}
}

void MXFile::SerializeHeader(ISerialize &Ar)
{
	if (Ar.IsSaving())
	{
		Ar << Id;
		Ar << URL;

		FResourceSummary *Header = GetSummary();
		if (Header)
		{
			Header->Serialize(Ar);
		}
		Ar << JsonStr;
	}
	else if (Ar.IsLoading())
	{
		Ar << Id;
		Ar << URL;

		FResourceSummary *Header = GetSummary();
		if (Header)
		{
			Header->Serialize(Ar);
			if (Header->HeadVersion > RESOURCE_HEADERVER_1)
			{
				Ar << JsonStr;
			}
		}
	}
}

IMeshObject *MXFile::GetMeshObject(int MeshIndex/* = 0*/, bool bBuildIfNotExist /*= true*/)
{
	IMeshObject *MeshObj = nullptr;
	if (MeshIndex >= 0 && MeshIndex < Meshes.size())
	{
		MeshObj = Meshes[MeshIndex];
	}
	return MeshObj;
}

bool MXFile::GetTextureInfo(int iTex, int &Width, int &Height, int &MipCount, int &Pitch, int &Format)
{
	if (iTex >= 0 && iTex < m_Textures.size())
	{
		FModelTexture *pTexture = m_Textures[iTex];
		if (pTexture)
		{
			Width = pTexture->Source.SizeX;
			Height = pTexture->Source.SizeY;
			MipCount = (int)pTexture->Source.CompressedImages.size();
			Pitch = Width * 2;
			Format = pTexture->Source.CompressedImages[0].PixelFormat;
			return true;
		}
	}
	return false;
}

bool MXFile::GetTextureData(int iTex, int MipIndex, void *&pData, int &nBytes)
{
	if (iTex >= 0 && iTex < m_Textures.size())
	{
		FModelTexture *pTexture = m_Textures[iTex];
		if (pTexture && MipIndex >= 0 && MipIndex < pTexture->Source.CompressedImages.size())
		{
			FTextureSourceInfo::FCompressedImage2D &Image = pTexture->Source.CompressedImages[MipIndex];
			pData = &Image.RawData[0];
			nBytes = (int)Image.RawData.size();
			return true;
		}
	}
	return false;
}


#define ADD_UNIQUE_VEC(vec, ivalue){	\
	size_t ik = 0;						\
	for (; ik < vec.size(); ++ik) {		\
		if (vec[ik] == ivalue) {		\
			break;						\
		}								\
	}									\
	if (ik >= vec.size()){				\
		vec.push_back(ivalue);			\
	}									\
}

void MXFile::ClearUnrefTexture()
{
	std::vector<int> UsedTextures;

	for (size_t i = 0; i < m_Materials.size(); ++i)
	{
		std::vector<FTexSlotInfo> RefTextures;
		SurfaceObject *material = m_Materials[i];
		if (material)
		{
			for (size_t i = 0; i < material->OverrideTextureParameters.size(); ++i)
			{
				int iTex = material->OverrideTextureParameters[i].Value;
				if (iTex >= 0)
				{
					ADD_UNIQUE_VEC(UsedTextures, iTex);
				}
			}
		}
	}

	std::vector<int> Remap;
	Remap.resize(m_Textures.size());	
	std::vector<FModelTexture*> newTextures;

	int k = 0;
	for (int i = 0; i < m_Textures.size(); ++i)
	{
		size_t ik = 0;
		for (; ik < UsedTextures.size(); ++ik)
		{
			if(UsedTextures[ik]==i)
			{
				break;
			}
		}

		if (ik < UsedTextures.size())
		{
			Remap[i] = newTextures.size();
			newTextures.push_back(m_Textures[i]);
		}
		else
		{
			if (m_Textures[i])
			{
				delete m_Textures[i];
				m_Textures[i] = 0;
			}
			Remap[i] = -1;
		}
	}

	m_Textures.swap(newTextures);

	for (size_t i = 0; i < m_Materials.size(); ++i)
	{
		std::vector<FTexSlotInfo> RefTextures;
		SurfaceObject *material = m_Materials[i];
		if (material)
		{
			for (size_t i = 0; i < material->OverrideTextureParameters.size(); ++i)
			{
				int iTex = material->OverrideTextureParameters[i].Value;
				if (iTex >= 0)
				{
					material->OverrideTextureParameters[i].Value = Remap[iTex];
				}
			}
		}
	}
}


