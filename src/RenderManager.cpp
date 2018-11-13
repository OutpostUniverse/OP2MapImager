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

void RenderManager::AddTileset(BYTE* tilesetMemoryPointer, std::size_t tilesetSize)
{
	FIMEMORY* fiMemory = FreeImage_OpenMemory(tilesetMemoryPointer, tilesetSize);

	try
	{
		if (FREE_IMAGE_FORMAT::FIF_BMP != FreeImage_GetFileTypeFromMemory(fiMemory, 0)) {
			throw std::runtime_error("Loaded an incorrect or invalid image type");
		}

		FreeImageBmp freeImageBmp(FREE_IMAGE_FORMAT::FIF_BMP, fiMemory);
		AddScaledTileset(freeImageBmp);
	}
	catch (const std::exception& e){
		FreeImage_CloseMemory(fiMemory);
		throw e;
	}

	FreeImage_CloseMemory(fiMemory);
}

void RenderManager::AddTileset(std::string filename, ImageFormat imageFormat)
{
	FreeImageBmp freeImageBmp(GetFIImageFormat(imageFormat), filename.c_str());

	AddScaledTileset(freeImageBmp);
}

void RenderManager::AddScaledTileset(const FreeImageBmp& fiTilesetBmp)
{
	const unsigned nonScaledTileLength = 32;
	const unsigned tilesetScaledWidth = fiTilesetBmp.Width() / nonScaledTileLength * scaleFactor;
	const unsigned tilesetScaledHeight = fiTilesetBmp.Height() / nonScaledTileLength * scaleFactor;

	tilesetBmps.push_back(fiTilesetBmp.Rescale(tilesetScaledWidth, tilesetScaledHeight));
}

void RenderManager::PasteTile(const int tilesetIndex, const int tileIndex, const int xPos, const int yPos)
{
	const int tilesetYPixelPos = tileIndex * scaleFactor;

	FreeImageBmp tileBmp = tilesetBmps[tilesetIndex].CreateView(
		0, tilesetYPixelPos + scaleFactor, scaleFactor, tilesetYPixelPos
	);

	const int leftPixelPos = xPos * scaleFactor;
	const int topPixelPos = yPos * scaleFactor;
	const int alpha = 256;

	try {
		tileBmp.Paste(freeImageBmpDest, leftPixelPos, topPixelPos, alpha);
	} catch(...) {
		throw std::runtime_error(
			"Unable to paste a tile index " + std::to_string(tileIndex) +
			" from tileset index " + std::to_string(tilesetIndex) + " onto new render"
		);
	}
}

void RenderManager::SaveMapImage(const std::string& destFilename, ImageFormat imageFormat)
{
	FREE_IMAGE_FORMAT fiImageFormat = GetFIImageFormat(imageFormat);
	int flags = GetFISaveFlag(fiImageFormat);
	freeImageBmpDest.Save(destFilename, fiImageFormat, flags);
}

FREE_IMAGE_FORMAT RenderManager::GetFIImageFormat(ImageFormat imageFormat) const
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
