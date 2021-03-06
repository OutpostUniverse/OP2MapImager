#pragma once

#include "FreeImageBmp.h"
#include "../FreeImage/Dist/x32/FreeImage.h"
#include <string>
#include <vector>
#include <cstddef>

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
	RenderManager(unsigned mapTileWidth, unsigned mapTileHeight, unsigned bpp, unsigned scaleFactor);

	void AddTileset(BYTE* tilesetMemoryPointer, std::size_t tilsesetSize);
	void AddTileset(std::string filename, ImageFormat imageFormat);

	void PasteTile(std::size_t tilesetIndex, std::size_t tileIndex, int xPos, int yPos);

	void SaveMapImage(const std::string& destFilename, ImageFormat imageFormat);

private:
	const unsigned scaleFactor;
	FreeImageBmp freeImageBmpDest;
	std::vector<FreeImageBmp> tilesetBmps;
	// The number of tiles contained in each tileset
	std::vector<unsigned> tilesetTileCounts;

	FREE_IMAGE_FORMAT GetFIImageFormat(ImageFormat imageFormat) const;
	int GetFISaveFlag(ImageFormat imageFormat) const;
	void AddScaledTileset(const FreeImageBmp& freeImageBmp);
};
