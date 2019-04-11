
#pragma once

#include "ResourceUtils.generated.h"

UCLASS(BlueprintType)
class UResourceUtils : public UObject
{
	GENERATED_UCLASS_BODY()
public:	
	UFUNCTION(BlueprintCallable, Category = "DR|EditorUtils", meta = (WorldContext = "Outer"))
	static UTextureRenderTarget2D *GetMaterialPreviewTexture(UObject *Outer, int32 SizeX, int32 SizeY, UMaterialInterface *Material);
	
	UFUNCTION(BlueprintCallable, Category = "DR|EditorUtils", meta = (WorldContext = "Outer"))
	static void SaveImage(UResource *Resource, UTextureRenderTarget2D *Texture);

	UFUNCTION(BlueprintCallable, Category = "DR|EditorUtils", meta = (WorldContext = "Outer"))
	static UTexture *GetImage(UResource *Resource);
	
	UFUNCTION(BlueprintCallable, Category = "DR|EditorUtils", meta = (WorldContext = "Outer"))
	static FString GetImagePath();
	static void SaveImage(UResource *Resource, int32 Width, int32 Height, const uint8 *Data, bool bSaveJpgOnly = false);
	static FString ExportPngImage(UResource *Resource, struct FModelTexture *texModel, const FString &TypeName);
	static FString ExportBmpImag(UResource *Resource, struct FModelTexture *texModel, const FString &TypeName);
	static FString ExportOpacityPngImage(UResource *Resource, struct FModelTexture *texModel, const FString &TypeName);
	static void GetPreviewMaterialImageSize(int32 &SizeX, int32 &SizeY);
private:
	static FString ImagePath;
};



