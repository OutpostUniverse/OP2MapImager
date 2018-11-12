#include "RenderManager.h"
#include <stdexcept>

using namespace std;

void RenderManager::Initialize()
{
	FreeImage_Initialise();
	FreeImage_SetOutputMessage(RenderManager::FreeImageErrorHandler);
}

void RenderManager::Deinitialize()
{
	FreeImage_DeInitialise();
}

void RenderManager::FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	printf("\n*** ");
	if (fif != FIF_UNKNOWN) {
		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
	}
	printf("%s", message);
	printf(" ***\n\n");
}

RenderManager::RenderManager(int mapTileWidth, int mapTileHeight, int bpp, int scaleFactor) : 
	scaleFactor(scaleFactor),
	freeImageBmpDest(mapTileWidth * scaleFactor, mapTileHeight * scaleFactor, bpp) { }

RenderManager::~RenderManager() 
{
	for (auto* fiBmp : tilesetBmps) {
		FreeImage_Unload(fiBmp);
	}
}

void RenderManager::AddTileset(BYTE* tilesetMemoryPointer, std::size_t tilesetSize)
{
	FIMEMORY* fiMemory = FreeImage_OpenMemory(tilesetMemoryPointer, tilesetSize);

	try
	{
		if (FREE_IMAGE_FORMAT::FIF_BMP != FreeImage_GetFileTypeFromMemory(fiMemory, 0)) {
			throw std::runtime_error("Loaded an incorrect or invalid image type");
		}

		FreeImageBmp freeImageBmp(FREE_IMAGE_FORMAT::FIF_BMP, fiMemory);
		ScaleTileset(freeImageBmp);
	}
	catch (const std::exception& e){
		FreeImage_CloseMemory(fiMemory);
		throw e;
	}

	FreeImage_CloseMemory(fiMemory);
}

void RenderManager::AddTileset(std::string filename, ImageFormat imageFormat)
{
	FreeImageBmp freeImageBmp(GetFiImageFormat(imageFormat), filename.c_str());

	ScaleTileset(freeImageBmp);
}

void RenderManager::ScaleTileset(FreeImageBmp& fiTilesetBmp)
{
	const unsigned nonScaledTileLength = 32;
	const unsigned tilesetScaledWidth = FreeImage_GetWidth(fiTilesetBmp.fiBitmap) / nonScaledTileLength * scaleFactor;
	const unsigned tilesetScaledHeight = FreeImage_GetHeight(fiTilesetBmp.fiBitmap) / nonScaledTileLength * scaleFactor;

	tilesetBmps.push_back(FreeImage_Rescale(fiTilesetBmp.fiBitmap, tilesetScaledWidth, tilesetScaledHeight));
}

void RenderManager::PasteTile(const int tilesetIndex, const int tileIndex, const int xPos, const int yPos)
{
	const int tilesetYPixelPos = tileIndex * scaleFactor;

	FIBITMAP* tileBmp = FreeImage_CreateView(tilesetBmps[tilesetIndex], 
		0, tilesetYPixelPos + scaleFactor, scaleFactor, tilesetYPixelPos);

	const int leftPixelPos = xPos * scaleFactor;
	const int topPixelPos = yPos * scaleFactor;
	const int alpha = 256;

	bool pasteSuccess = FreeImage_Paste(freeImageBmpDest.fiBitmap, tileBmp, leftPixelPos, topPixelPos, alpha);

	if (!pasteSuccess) {
		throw std::runtime_error("Unable to paste a tile index " + std::to_string(tileIndex) + 
			" from tileset index " + std::to_string(tilesetIndex) + " onto new render");
	}

	FreeImage_Unload(tileBmp);
}

bool RenderManager::SaveMapImage(const std::string& destFilename, ImageFormat imageFormat)
{
	FREE_IMAGE_FORMAT fiImageFormat = GetFiImageFormat(imageFormat);

	return FreeImage_Save(fiImageFormat, freeImageBmpDest.fiBitmap, destFilename.c_str(), GetFISaveFlag(fiImageFormat));
}

FREE_IMAGE_FORMAT RenderManager::GetFiImageFormat(ImageFormat imageFormat) const
{
	switch (imageFormat)
	{
	case ImageFormat::BMP:
		return FREE_IMAGE_FORMAT::FIF_BMP;
	case ImageFormat::JPG:
		return FREE_IMAGE_FORMAT::FIF_JPEG;
	case ImageFormat::PNG:
		return FREE_IMAGE_FORMAT::FIF_PNG;
	default:
		return FREE_IMAGE_FORMAT::FIF_BMP;
	}
}

int RenderManager::GetFISaveFlag(FREE_IMAGE_FORMAT imageFormat) const
{
	switch (imageFormat)
	{
	case FIF_BMP:
		return BMP_DEFAULT;
	case FIF_JPEG:
		return JPEG_DEFAULT;
	case FIF_PNG:
		return PNG_DEFAULT;
	default:
		return BMP_DEFAULT;
	}
}
