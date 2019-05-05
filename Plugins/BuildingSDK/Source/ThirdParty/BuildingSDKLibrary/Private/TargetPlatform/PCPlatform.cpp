
#include "PCPlatform.h"
#include "MXFile/MXFile.h"
#include "IBuildingSDK.h"

#define RES_SURFACE	0
#define RES_MODEL	1

extern void SaveMark(char resType, ISerialize &Ar);

void PC_CookMXFfile(MXFile *mxFile, const char *OutputFilename)
{
	if (mxFile)
	{
		FResourceSummary *pSummary = mxFile->GetSummary();
		if (pSummary)
		{
			pSummary->Image.SizeX = 0;
			pSummary->Image.SizeY = 0;
			pSummary->Image.Data.clear();
		}
		
		mxFile->ClearUnrefTexture();

		for (size_t i = 0; i<mxFile->m_Textures.size(); ++i)
		{
			FModelTexture *tex = mxFile->m_Textures[i];
			if (tex)
			{
				tex->Source.StripRawData();
			}
		}

		IBuildingSDK *SDK = GetBuildingSDK();
		if (SDK)
		{
			ISerialize *Writer = SDK->CreateFileWriter(OutputFilename);
			if (Writer)
			{
				SaveMark(RES_MODEL, *Writer);
				mxFile->Serialize(*Writer);
				Writer->Close();
			}
		}
	}
}

bool PCPlatform::Cook(IObject *Object, const char *OutputFilename)
{
	if (Object->IsA(EMXFile))
	{
		PC_CookMXFfile((MXFile *)Object, OutputFilename);
	}
	return true;
}

