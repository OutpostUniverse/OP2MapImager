#pragma once

#include "FreeImage/Dist/x32/FreeImage.h"
#include <string>
#include <vector>

enum class ImageFormat
{
	BMP,
	JPG,
	PNG,
};

class RenderManager
{
	// fi stands for Free Image.
	// dib stands for device independent bitmap.

public:
	static void Initialize();
	static void Deinitialize();

	// Prints Error messages to console generated by FreeImage
	static void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message);

	// ScaleFactor is the width/height in pixels of each tile.
	RenderManager(int mapTileWidth, int mapTileHeight, int bpp, int scaleFactor);

	~RenderManager();

	void AddTilesetRawBits(BYTE* bits, int width, int height, int pitch, unsigned bpp,
		unsigned red_mask, unsigned green_mask, unsigned blue_mask);

	void AddTileset(std::string filename, ImageFormat imageFormat);

	void PasteTile(int tilesetIndex, int tileIndex, int xPos, int yPos);

	// Returns true on success.
	bool SaveMapImage(const std::string& destFilename, ImageFormat imageFormat);

private:
	int scaleFactor;
	FIBITMAP* fiBmpDest;
	std::vector<FIBITMAP*> tilesetBmps;

	FREE_IMAGE_FORMAT GetFiImageFormat(ImageFormat imageFormat);
	int GetFISaveFlag(FREE_IMAGE_FORMAT imageFormat);
	void ScaleTileset(FIBITMAP* fiTilesetBmp);
};
