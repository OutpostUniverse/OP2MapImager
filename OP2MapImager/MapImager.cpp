#include "MapImager.h"

Rectangle::Rectangle(void) { }

Rectangle::Rectangle(int xVal, int yVal, int widthVal, int heightVal)
{
	x = xVal;
	y = yVal;
	width = widthVal;
	height = heightVal;
}

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

MapImager::MapImager(int width, int height, int bpp, int outputTilePixelSize)
{
	this->outputTilePixelSize = outputTilePixelSize;
	fiBmpDest = FreeImage_Allocate(width, height, bpp);
}

MapImager::~MapImager() {
	for (FIBITMAP* fiBmp : tileSetBmps)
		FreeImage_Unload(fiBmp);

	FreeImage_Unload(fiBmpDest);
}

void MapImager::AddTileSetRawBits(BYTE* bits, int width, int height, int pitch, unsigned bpp,
	unsigned red_mask, unsigned green_mask, unsigned blue_mask)
{
	FIBITMAP* fiTileSetBmp = FreeImage_ConvertFromRawBits(bits, width, height, pitch,
		bpp, red_mask, green_mask, blue_mask);

	tileSetBmps.push_back(fiTileSetBmp);
}

void MapImager::ScaleAndPaste(int tileSetIndex, Rectangle sourceRect, int destTopLeftPixel, int destTopRightPixel)
{
	//TODO: Consider pre-scaling tile set images.

	//FreeImage_Copy
	FIBITMAP* bmpScaled = FreeImage_RescaleRect(tileSetBmps[tileSetIndex], outputTilePixelSize, outputTilePixelSize,
		sourceRect.Left(), sourceRect.Top(), sourceRect.Right(), sourceRect.Bottom());

	int alpha = 256;
	FreeImage_Paste(fiBmpDest, bmpScaled, destTopLeftPixel, destTopRightPixel, alpha);

	FreeImage_Unload(bmpScaled);
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