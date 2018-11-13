#pragma once

#include "../FreeImage/Dist/x32/FreeImage.h"
#include <string>

// Wrapper for FIBITMAP to ensure proper destruction
class FreeImageBmp
{
public:
	// Create new bitmap from raw FreeImage bitmap handle
	FreeImageBmp(FIBITMAP* fiBitmap);

	FreeImageBmp(FreeImageBmp&& other); // Move constructor

	// Create a new, default bitmap (pixels are all black)
	FreeImageBmp(int width, int height, int bpp);

	// Create bitmap from memory buffer
	FreeImageBmp(FREE_IMAGE_FORMAT imageType, FIMEMORY* fiMemory);

	// Create a bitmap from a file
	FreeImageBmp(FREE_IMAGE_FORMAT imageFormat, const std::string& filename);

	~FreeImageBmp();

	// Get image dimensions
	unsigned Width() const;
	unsigned Height() const;

	// Create a rescaled bitmap
	FreeImageBmp Rescale(int scaledWidth, int scaledHeight) const;

	// Create view into bitmap
	FreeImageBmp CreateView(unsigned left, unsigned top, unsigned right, unsigned bottom) const;

	FIBITMAP* fiBitmap;
};
