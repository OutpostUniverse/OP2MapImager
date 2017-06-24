#include "MapImager.h"

void MapImager::FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	printf("\n*** ");
	if (fif != FIF_UNKNOWN) {
		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
	}
	printf(message);
	printf(" ***\n");
}

void MapImager::Initialize()
{
	FreeImage_Initialise();
	FreeImage_SetOutputMessage(MapImager::FreeImageErrorHandler);
}

void MapImager::DeInitialize()
{
	FreeImage_DeInitialise();
}

MapImager::MapImager(int mapTileWidth, int mapTileHeight, int bpp, int scaleFactor)
{
	this->scaleFactor = scaleFactor;
	fiBmpDest = FreeImage_Allocate(mapTileWidth * scaleFactor, mapTileHeight * scaleFactor, bpp);
}

MapImager::~MapImager() {
	for (FIBITMAP* fiBmp : tileSetBmps)
		FreeImage_Unload(fiBmp);

	FreeImage_Unload(fiBmpDest);
}

void MapImager::ScaleTileSet(FIBITMAP* fiTileSetBmp)
{
	unsigned nonScaledTileLength = 32;
	unsigned imageWidth = FreeImage_GetWidth(fiTileSetBmp);
	unsigned imageHeight = FreeImage_GetHeight(fiTileSetBmp);
	unsigned tileSetScaledWidth = FreeImage_GetWidth(fiTileSetBmp) / nonScaledTileLength * scaleFactor;
	unsigned tileSetScaledHeight = FreeImage_GetHeight(fiTileSetBmp) / nonScaledTileLength * scaleFactor;

	tileSetBmps.push_back(FreeImage_Rescale(fiTileSetBmp, tileSetScaledWidth, tileSetScaledHeight));

	FreeImage_Unload(fiTileSetBmp);
}

void MapImager::AddTileSetRawBits(BYTE* bits, int width, int height, int pitch, unsigned bpp,
	unsigned red_mask, unsigned green_mask, unsigned blue_mask)
{
	FIBITMAP* fiTileSetBmp = FreeImage_ConvertFromRawBits(bits, width, height, pitch,
		bpp, red_mask, green_mask, blue_mask);

	ScaleTileSet(fiTileSetBmp);
}

void MapImager::AddTileSet(std::string filename, ImageFormat imageFormat)
{
	FIBITMAP* fiTileSetBmp = FreeImage_Load(GetFiImageFormat(imageFormat), filename.c_str());

	ScaleTileSet(fiTileSetBmp);
}

void MapImager::PasteTile(int tileSetIndex, int tileIndex, int xPos, int yPos)
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

bool MapImager::SaveMapImage(const std::string& destFilename, ImageFormat imageFormat)
{
	FREE_IMAGE_FORMAT fiImageFormat = GetFiImageFormat(imageFormat);

	return FreeImage_Save(fiImageFormat, fiBmpDest, destFilename.c_str(), GetFiSaveFlag(fiImageFormat));
}

FREE_IMAGE_FORMAT MapImager::GetFiImageFormat(ImageFormat imageFormat)
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

int MapImager::GetFiSaveFlag(FREE_IMAGE_FORMAT imageFormat)
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