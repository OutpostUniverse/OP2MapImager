#include "RenderManager.h"

using namespace std;

void RenderManager::freeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	printf("\n*** ");
	if (fif != FIF_UNKNOWN) {
		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
	}
	printf("%s", message);
	printf(" ***\n\n");
}

void RenderManager::initialize()
{
	FreeImage_Initialise();
	FreeImage_SetOutputMessage(RenderManager::freeImageErrorHandler);
}

void RenderManager::deInitialize()
{
	FreeImage_DeInitialise();
}

RenderManager::RenderManager(int mapTileWidth, int mapTileHeight, int bpp, int scaleFactor)
{
	this->scaleFactor = scaleFactor;
	fiBmpDest = FreeImage_Allocate(mapTileWidth * scaleFactor, mapTileHeight * scaleFactor, bpp);
}

RenderManager::~RenderManager() 
{
	for (FIBITMAP* fiBmp : tileSetBmps) {
		FreeImage_Unload(fiBmp);
	}

	FreeImage_Unload(fiBmpDest);
}

void RenderManager::scaleTileSet(FIBITMAP* fiTileSetBmp)
{
	unsigned nonScaledTileLength = 32;
	unsigned imageWidth = FreeImage_GetWidth(fiTileSetBmp);
	unsigned imageHeight = FreeImage_GetHeight(fiTileSetBmp);
	unsigned tileSetScaledWidth = FreeImage_GetWidth(fiTileSetBmp) / nonScaledTileLength * scaleFactor;
	unsigned tileSetScaledHeight = FreeImage_GetHeight(fiTileSetBmp) / nonScaledTileLength * scaleFactor;

	tileSetBmps.push_back(FreeImage_Rescale(fiTileSetBmp, tileSetScaledWidth, tileSetScaledHeight));

	FreeImage_Unload(fiTileSetBmp);
}

void RenderManager::addTileSetRawBits(BYTE* bits, int width, int height, int pitch, unsigned bpp,
	unsigned red_mask, unsigned green_mask, unsigned blue_mask)
{
	FIBITMAP* fiTileSetBmp = FreeImage_ConvertFromRawBits(bits, width, height, pitch,
		bpp, red_mask, green_mask, blue_mask);

	scaleTileSet(fiTileSetBmp);
}

void RenderManager::addTileSet(std::string filename, ImageFormat imageFormat)
{
	FIBITMAP* fiTileSetBmp = FreeImage_Load(GetFiImageFormat(imageFormat), filename.c_str());

	scaleTileSet(fiTileSetBmp);
}

void RenderManager::pasteTile(int tileSetIndex, int tileIndex, int xPos, int yPos)
{
	int tileSetYPixelPos = tileIndex * scaleFactor;

	FIBITMAP* tileBmp = FreeImage_CreateView(tileSetBmps[tileSetIndex], 
		0, tileSetYPixelPos + scaleFactor, scaleFactor, tileSetYPixelPos);

	int viewWidth = FreeImage_GetWidth(tileBmp);
	int viewHeight = FreeImage_GetHeight(tileBmp);

	int leftPixelPos = xPos * scaleFactor;
	int topPixelPos = yPos * scaleFactor;

	int alpha = 256;
	bool pasteSuccess = FreeImage_Paste(fiBmpDest, tileBmp, leftPixelPos, topPixelPos, alpha);

	FreeImage_Unload(tileBmp);
}

bool RenderManager::saveMapImage(const std::string& destFilename, ImageFormat imageFormat)
{
	FREE_IMAGE_FORMAT fiImageFormat = GetFiImageFormat(imageFormat);

	return FreeImage_Save(fiImageFormat, fiBmpDest, destFilename.c_str(), getFISaveFlag(fiImageFormat));
}

FREE_IMAGE_FORMAT RenderManager::GetFiImageFormat(ImageFormat imageFormat)
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

int RenderManager::getFISaveFlag(FREE_IMAGE_FORMAT imageFormat)
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
