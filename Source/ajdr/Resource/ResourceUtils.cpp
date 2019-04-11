
#include "ajdr.h"
#include "ResourceUtils.h"
#include "DRGameMode.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "DRPreviewScene.h"
#include "RenderingThread.h"
#include "Resource.h"
#include "ModelTexture.h"
#include "BmpImageSupport.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

FString UResourceUtils::ImagePath = "";

UResourceUtils::UResourceUtils(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

UTextureRenderTarget2D *UResourceUtils::GetMaterialPreviewTexture(UObject *Outer, int32 SizeX, int32 SizeY, UMaterialInterface *Material)
{
	ADRGameMode *MyGame = nullptr;
	FDRMaterialScene *pDRMtrlScene = nullptr;

	UWorld *MyWorld = Outer->GetWorld();
	if (MyWorld)
	{
		MyGame = Cast<ADRGameMode> (MyWorld->GetAuthGameMode());
		pDRMtrlScene = MyGame->GetMaterialPreviewScene();
	}

	if (pDRMtrlScene)
	{
		UTextureRenderTarget2D* TexRenderTarget = NewObject<UTextureRenderTarget2D>(Outer);

		check(TexRenderTarget);
		TexRenderTarget->InitAutoFormat(SizeX, SizeY);
		TexRenderTarget->UpdateResourceImmediate(true);

		if (TexRenderTarget && pDRMtrlScene)
		{
			//设置场景截图灯光
			pDRMtrlScene->SetLightDirection(FRotator(30, -70, 0));
			pDRMtrlScene->SetMaterialInterface(Material, MyGame->GetPreviewMaterialMesh());

			UWorld *World = pDRMtrlScene->GetWorld();

			FCanvas RenderCanvas( TexRenderTarget->GameThread_GetRenderTargetResource(),
								  nullptr,
								  World,
								  World->FeatureLevel);

			FTextureRenderTargetResource *RenderTarget = TexRenderTarget->GameThread_GetRenderTargetResource();
			pDRMtrlScene->Draw(SizeX, SizeY, RenderTarget, &RenderCanvas);

			{
				RenderCanvas.Flush_GameThread();

				ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER (
															UpdateMaterialPreviewRTCommand,
															FTextureRenderTargetResource*, RenderTargetResource, RenderTarget,
															{
																RHICmdList.CopyToResolveTarget(							// Copy (resolve) the rendered thumbnail from the render target to its texture
																	RenderTargetResource->GetRenderTargetTexture(),		// Source texture
																	RenderTargetResource->TextureRHI,					// Dest texture
																	false,												// Do we need the source image content again?
																	FResolveParams());									// Resolve parameters
															});
			}
		}

		return TexRenderTarget;
	}

	return NULL;
}

FString UResourceUtils::GetImagePath()
{
	return ImagePath;
}

void UResourceUtils::SaveImage(UResource *Resource, int32 Width, int32 Height, const uint8 *Data, bool bSaveJpgOnly /*= false*/)
{
	if (Data && Resource)
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
		if (ImageWrapper.IsValid())
		{
			ImageWrapper->SetRaw(Data, Width * Height * sizeof(FColor), Width, Height, ERGBFormat::BGRA, 8);
			FString Filename = FString::Printf(TEXT("%sImage/%s.jpg"), *FPaths::ProjectSavedDir(), *FPaths::GetBaseFilename(Resource->Filename, true));
			FFileHelper::SaveArrayToFile(ImageWrapper->GetCompressed(100), *Filename);
			ImagePath = Filename;
		}

		if (!bSaveJpgOnly)
		{
			FResourceSummary *Header = Resource->GetSummary();
			if (Header)
			{
				Header->Image.SizeX = Width;
				Header->Image.SizeY = Height;
				int32 Sz = Width * Height * sizeof(FColor);
				Header->Image.Data.SetNum(Sz);
				FMemory::Memcpy(Header->Image.Data.GetData(), Data, Sz);
				Resource->MarkDirty();
				Resource->Save();
			}
		}
	}
}

FString UResourceUtils::ExportPngImage(UResource *Resource, FModelTexture *texModel, const FString &TypeName)
{
	FString Filename;

	if (Resource && texModel)
	{
		uint8 *pData = texModel->Source.Data.GetData();
		
		int32 Width = texModel->Source.SizeX;
		int32 Height = texModel->Source.SizeY;

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
		if (ImageWrapper.IsValid())
		{
			ERGBFormat RGBFormat = ERGBFormat::BGRA;
			int32 BytesPrePixel = sizeof(FColor);

			if (texModel->Source.SourceFormat == TSF_G8)
			{
				RGBFormat = ERGBFormat::Gray;
				BytesPrePixel = 1;
			}

			ImageWrapper->SetRaw(pData, Width * Height * BytesPrePixel, Width, Height, RGBFormat, 8);
			Filename = FString::Printf(TEXT("%sImage/%s_%s.png"), *FPaths::ProjectSavedDir(), *FPaths::GetBaseFilename(Resource->Filename, true), *TypeName);
			FFileHelper::SaveArrayToFile(ImageWrapper->GetCompressed(100), *Filename);
			Filename = FPaths::ConvertRelativePathToFull(Filename);
		}
	}
	return Filename;
}

FString UResourceUtils::ExportBmpImag(UResource *Resource, FModelTexture *texModel, const FString &TypeName)
{
	FString Filename;

	if (Resource && texModel)
	{
		if (texModel->Source.SourceFormat == TSF_G8)
		{
			return ExportPngImage(Resource, texModel, TypeName);
		}

		uint8 *pData = texModel->Source.Data.GetData();

		int32 Width = texModel->Source.SizeX;
		int32 Height = texModel->Source.SizeY;

		FString SaveDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
		Filename = FString::Printf(TEXT("%sImage/%s_%s.bmp"), *SaveDir, *FPaths::GetBaseFilename(Resource->Filename, true), *TypeName);
		FArchive* Ar = IFileManager::Get().CreateFileWriter(*Filename, 0);

		FBitmapFileHeader bmf;
		FBitmapInfoHeader bmhdr;

		// File header.
		bmf.bfType = 'B' + (256 * (int32)'M');
		bmf.bfReserved1 = 0;
		bmf.bfReserved2 = 0;
		int32 biSizeImage = Width * Height * 4;
		bmf.bfOffBits = sizeof(FBitmapFileHeader) + sizeof(FBitmapInfoHeader);
		bmhdr.biBitCount = 32;

		bmf.bfSize = bmf.bfOffBits + biSizeImage;
		*Ar << bmf;

		// Info header.
		bmhdr.biSize = sizeof(FBitmapInfoHeader);
		bmhdr.biWidth = Width;
		bmhdr.biHeight = -Height;
		bmhdr.biPlanes = 1;
		bmhdr.biCompression = BCBI_RGB;
		bmhdr.biSizeImage = biSizeImage;
		bmhdr.biXPelsPerMeter = 0;
		bmhdr.biYPelsPerMeter = 0;
		bmhdr.biClrUsed = 0;
		bmhdr.biClrImportant = 0;
		*Ar << bmhdr;


		Ar->Serialize(const_cast<uint8*>(texModel->Source.Data.GetData()), texModel->Source.Data.Num());
		delete Ar;
	}
	else
	{
		Filename = TEXT("");
	}
	return Filename;
}

FString UResourceUtils::ExportOpacityPngImage(UResource *Resource, FModelTexture *texModel, const FString &TypeName)
{
	FString Filename;

	if (Resource && texModel)
	{
		uint8 *pData = texModel->Source.Data.GetData();

		int32 Width = texModel->Source.SizeX;
		int32 Height = texModel->Source.SizeY;

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
		if (ImageWrapper.IsValid())
		{
			TArray<uint8> Data;
			Data.SetNum(Width * Height);

			if (texModel->Source.SourceFormat != TSF_G8)
			{
				FColor *pSrc = (FColor *)pData;
				uint8 *pDst = Data.GetData();
				for (int32 iPixel = 0; iPixel < Width*Height; ++iPixel)
				{
					pDst[iPixel] = pSrc[iPixel].A;
				}
				pData = pDst;
			}

			const int32 BytesPrePixel = 1;

			ImageWrapper->SetRaw(pData, Width * Height * BytesPrePixel, Width, Height, ERGBFormat::Gray, 8);
			Filename = FString::Printf(TEXT("%sImage/%s_%s.png"), *FPaths::ProjectSavedDir(), *FPaths::GetBaseFilename(Resource->Filename, true), *TypeName);
			FFileHelper::SaveArrayToFile(ImageWrapper->GetCompressed(100), *Filename);
			Filename = FPaths::ConvertRelativePathToFull(Filename);
		}
	}
	return Filename;
}

void UResourceUtils::SaveImage(UResource *Resource, UTextureRenderTarget2D *Texture)
{
	if (Resource && Texture)
	{
		FTextureRenderTargetResource *RenderTarget = Texture->GameThread_GetRenderTargetResource();
		if (Resource != NULL)
		{
			TArray<FColor> Colors;
			if (RenderTarget->ReadPixels(Colors))
			{
				int32 Width = Texture->GetSurfaceWidth();
				int32 Height = Texture->GetSurfaceHeight();
				SaveImage(Resource, Width, Height, (uint8 *)Colors.GetData());
			}
		}
	}
}

UTexture *UResourceUtils::GetImage(UResource *Resource)
{
	UTexture *Texture = NULL;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	if (Resource && ImageWrapper.IsValid())
	{
		FString Filename = FString::Printf(TEXT("%sImage/%s.jpg"), *FPaths::ProjectSavedDir(), *FPaths::GetBaseFilename(Resource->Filename, true));
		ImagePath = Filename;

		TArray<uint8> CompressedData;
		if (FFileHelper::LoadFileToArray(CompressedData, *Filename))
		{
			if (ImageWrapper->SetCompressed(CompressedData.GetData(), CompressedData.GetAllocatedSize()))
			{
				const TArray<uint8> *Data = NULL;
				if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, Data))
				{
					int32 SizeX = ImageWrapper->GetWidth();
					int32 SizeY = ImageWrapper->GetHeight();

					UTexture2D *Tex2D = UTexture2D::CreateTransient(SizeX, SizeY, EPixelFormat::PF_B8G8R8A8);
					if (Tex2D)
					{
						FTexture2DMipMap &MipData = Tex2D->PlatformData->Mips[0];
						void *pData = MipData.BulkData.Lock(LOCK_READ_WRITE);
						if (pData)
						{
							FMemory::Memcpy(pData, Data->GetData(), Data->Num());
							MipData.BulkData.Unlock();
							Tex2D->UpdateResource();
						}
						Texture = Tex2D;
					}
				}
			}
		}
	}
	
	return Texture;
}


void UResourceUtils::GetPreviewMaterialImageSize(int32 &SizeX, int32 &SizeY)
{ 
	SizeX = 128; 
	SizeY = 128;
}


