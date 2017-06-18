#pragma once

#include "FreeImage/FreeImage.h"
#include <string>
#include <vector>

enum class ImageFormat
{
	BMP,
	JPG,
	PNG,
};

struct Rectangle
{
public:
	Rectangle();
	Rectangle(int x, int y, int width, int height);
	int x;
	int y;
	int width;
	int height;

	int Left() {
		return x;
	}

	int Right(){
		return x + width;
	}

	int Top() {
		return y;
	}

	int Bottom() {
		return y + height;
	}
};

class MapImager
{
	// fi stands for Free Image.
	// dib stands for device independent bitmap

public:
	static void Initialize();
	static void DeInitialize();

	// Prints Error messages to console generated by FreeImage
	static void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message);

	MapImager(int width, int height, int bpp, int outputTilePixelSize);

	~MapImager();

	void AddTileSetRawBits(BYTE* bits, int width, int height, int pitch, unsigned bpp,
		unsigned red_mask, unsigned green_mask, unsigned blue_mask);

	void ScaleAndPaste(int tileSetIndex, Rectangle sourceRect, int destTopLeftPixel, int destTopRightPixel);

	// Returns true on success.
	bool SaveMapImage(const std::string& destFilename, ImageFormat imageFormat);

private:
	int outputTilePixelSize;
	FIBITMAP* fiBmpDest;
	std::vector<FIBITMAP*> tileSetBmps;

	FREE_IMAGE_FORMAT GetFiImageFormat(ImageFormat imageFormat);
	int GetFiSaveFlag(FREE_IMAGE_FORMAT imageFormat);
};