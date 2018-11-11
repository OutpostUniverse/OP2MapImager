#include "RenderManager.h"

using namespace std;

void RenderManager::FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	printf("\n*** ");
	if (fif != FIF_UNKNOWN) {
		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
	}
	printf("%s", message);
	printf(" ***\n\n");
}

void RenderManager::Initialize()
{
	FreeImage_Initialise();
	FreeImage_SetOutputMessage(RenderManager::FreeImageErrorHandler);
}

void RenderManager::Deinitialize()
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
	for (auto* fiBmp : tilesetBmps) {
		FreeImage_Unload(fiBmp);
	}

	FreeImage_Unload(fiBmpDest);
}

void RenderManager::ScaleTileset(FIBITMAP* fiTilesetBmp)
{
	unsigned nonScaledTileLength = 32;
	unsigned imageWidth = FreeImage_GetWidth(fiTilesetBmp);
	unsigned imageHeight = FreeImage_GetHeight(fiTilesetBmp);
	unsigned tilesetScaledWidth = FreeImage_GetWidth(fiTilesetBmp) / nonScaledTileLength * scaleFactor;
	unsigned tilesetScaledHeight = FreeImage_GetHeight(fiTilesetBmp) / nonScaledTileLength * scaleFactor;

	tilesetBmps.push_back(FreeImage_Rescale(fiTilesetBmp, tilesetScaledWidth, tilesetScaledHeight));
	FreeImage_Unload(fiTilesetBmp);
}

void RenderManager::AddTileset(BYTE* tilesetMemoryPointer, std::size_t tilesetSize)
{
	FIMEMORY* fiMemory = FreeImage_OpenMemory(tilesetMemoryPointer, tilesetSize);

	try
	{
		// get the file type
		FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(fiMemory, 0);

		if (fif != FREE_IMAGE_FORMAT::FIF_BMP) {
			throw std::runtime_error("OP2MapImager only supports loading tilesets that are bitmaps");
		}

		// load an image from the memory stream
		FIBITMAP* fiBitMap = FreeImage_LoadFromMemory(fif, fiMemory, 0);

		ScaleTileset(fiBitMap);
	}
	catch (const std::exception& e){
		FreeImage_CloseMemory(fiMemory);
		throw e;
	}

	FreeImage_CloseMemory(fiMemory);
}

void RenderManager::AddTileset(std::string filename, ImageFormat imageFormat)
{
	FIBITMAP* fiTilesetBmp = FreeImage_Load(GetFiImageFormat(imageFormat), filename.c_str());

	ScaleTileset(fiTilesetBmp);
}

void RenderManager::PasteTile(int tilesetIndex, int tileIndex, int xPos, int yPos)
{
	int tilesetYPixelPos = tileIndex * scaleFactor;

	FIBITMAP* tileBmp = FreeImage_CreateView(tilesetBmps[tilesetIndex], 
		0, tilesetYPixelPos + scaleFactor, scaleFactor, tilesetYPixelPos);

	int viewWidth = FreeImage_GetWidth(tileBmp);
	int viewHeight = FreeImage_GetHeight(tileBmp);

	int leftPixelPos = xPos * scaleFactor;
	int topPixelPos = yPos * scaleFactor;

	int alpha = 256;
	bool pasteSuccess = FreeImage_Paste(fiBmpDest, tileBmp, leftPixelPos, topPixelPos, alpha);

	FreeImage_Unload(tileBmp);
}

bool RenderManager::SaveMapImage(const std::string& destFilename, ImageFormat imageFormat)
{
	FREE_IMAGE_FORMAT fiImageFormat = GetFiImageFormat(imageFormat);

	return FreeImage_Save(fiImageFormat, fiBmpDest, destFilename.c_str(), GetFISaveFlag(fiImageFormat));
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

int RenderManager::GetFISaveFlag(FREE_IMAGE_FORMAT imageFormat)
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
