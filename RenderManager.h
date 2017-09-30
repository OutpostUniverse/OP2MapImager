#pragma once

#include "FreeImage.h"
#include <string>
#include <vector>

using namespace std;

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
	static void initialize();
	static void deInitialize();

	// Prints Error messages to console generated by FreeImage
	static void freeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message);

	// ScaleFactor is the width/height in pixels of each tile.
	RenderManager(int mapTileWidth, int mapTileHeight, int bpp, int scaleFactor);

	~RenderManager();

	void addTileSetRawBits(BYTE* bits, int width, int height, int pitch, unsigned bpp,
		unsigned red_mask, unsigned green_mask, unsigned blue_mask);

	void addTileSet(std::string filename, ImageFormat imageFormat);

	void pasteTile(int tileSetIndex, int tileIndex, int xPos, int yPos);

	// Returns true on success.
	bool saveMapImage(const string& destFilename, ImageFormat imageFormat);

private:
	int scaleFactor;
	FIBITMAP* fiBmpDest;
	std::vector<FIBITMAP*> tileSetBmps;

	FREE_IMAGE_FORMAT GetFiImageFormat(ImageFormat imageFormat);
	int getFISaveFlag(FREE_IMAGE_FORMAT imageFormat);
	void scaleTileSet(FIBITMAP* fiTileSetBmp);
};