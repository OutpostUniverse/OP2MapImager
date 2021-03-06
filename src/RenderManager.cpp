#include "RenderManager.h"
#include <stdexcept>
#include <limits>

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

RenderManager::RenderManager(unsigned mapTileWidth, unsigned mapTileHeight, unsigned bpp, unsigned scaleFactor) : 
	scaleFactor(scaleFactor),
	freeImageBmpDest(mapTileWidth * scaleFactor, mapTileHeight * scaleFactor, bpp) 
{
	// maxTileDimension is the maximum width or height of a map in tiles
	const auto maxTileDimension = std::numeric_limits<unsigned int>::max() / scaleFactor;
	if ((mapTileWidth > maxTileDimension) || (mapTileHeight > maxTileDimension)) {
		throw std::runtime_error("Provided scale factor is too large to render a map with this tile width and height");
	}
}

void RenderManager::AddTileset(BYTE* tilesetMemoryPointer, std::size_t tilesetSize)
{
	if (tilesetSize > std::numeric_limits<DWORD>::max()) {
		throw std::runtime_error("Tileset size is too large");
	}

	FIMEMORY* fiMemory = FreeImage_OpenMemory(tilesetMemoryPointer, static_cast<DWORD>(tilesetSize));

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

	// Only source tilesets of standard width are supported
	if (fiTilesetBmp.Width() != nonScaledTileLength) {
		throw std::runtime_error("Source tileset width must match " + 
			std::to_string(nonScaledTileLength) + " pixels (1 tile)");
	}
	// Height must be a multiple of the tile size
	if ((fiTilesetBmp.Width() % nonScaledTileLength) != 0) {
		throw std::runtime_error("Source tileset height must be an integer multiple of " + 
			std::to_string(nonScaledTileLength) + " pixels (tile size)");
	}

	// Determine number of tiles
	const unsigned tilesetTileCount = fiTilesetBmp.Height() / nonScaledTileLength;

	// Pre-check scaled tileset size
	const unsigned maxScaledTiles = std::numeric_limits<unsigned>::max() / scaleFactor;
	if (tilesetTileCount > maxScaledTiles) {
		throw std::runtime_error("Scaled tileset height exceeds memory size");
	}

	// Calculate scaled tileset dimensions
	const unsigned tilesetScaledWidth = scaleFactor;
	const unsigned tilesetScaledHeight = tilesetTileCount * scaleFactor;

	tilesetBmps.push_back(fiTilesetBmp.Rescale(tilesetScaledWidth, tilesetScaledHeight));
	tilesetTileCounts.push_back(tilesetTileCount);
}

void RenderManager::PasteTile(std::size_t tilesetIndex, std::size_t tileIndex, int xPos, int yPos)
{
	if (tilesetIndex >= tilesetTileCounts.size()) {
		throw std::runtime_error("Requested tileset has not been loaded into RenderManager");
	}

	// Check tile index values are in range
	if (tileIndex >= tilesetTileCounts[tilesetIndex]) {
		throw std::runtime_error("Tile index out of range");
	}

	// Image dimension pre-checked, so no overflow if tileIndex is in range
	const unsigned int tilesetYPixelPos = static_cast<unsigned int>(tileIndex * scaleFactor);

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
	freeImageBmpDest.Save(destFilename, GetFIImageFormat(imageFormat), GetFISaveFlag(imageFormat));
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

int RenderManager::GetFISaveFlag(ImageFormat imageFormat) const
{
	switch (imageFormat)
	{
	case ImageFormat::BMP:
		return BMP_DEFAULT;
	case ImageFormat::JPG:
		return JPEG_DEFAULT;
	case ImageFormat::PNG:
		return PNG_DEFAULT;
	default:
		return BMP_DEFAULT;
	}
}
