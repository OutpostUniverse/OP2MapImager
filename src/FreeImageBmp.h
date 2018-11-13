#pragma once

#include "../FreeImage/Dist/x32/FreeImage.h"
#include <string>

// Wrapper for FIBITMAP to ensure proper destruction
class FreeImageBmp
{
public:
	FreeImageBmp(FreeImageBmp&& other); // Move constructor

	// Create a new, default bitmap (pixels are all black)
	FreeImageBmp(int width, int height, int bpp);

	// Create bitmap from memory stream
	FreeImageBmp(FREE_IMAGE_FORMAT imageType, FIMEMORY* fiMemory);

	// Create a bitmap from a file
	FreeImageBmp(FREE_IMAGE_FORMAT imageFormat, const std::string& filename);

	// Create a new bitmap by scaling the size and width of an existing bitmap
	FreeImageBmp(const FreeImageBmp& freeImageBmp, int scaledWidth, int scaledHeight);

	~FreeImageBmp();

	FIBITMAP* fiBitmap;
};
