#pragma once

#include "../FreeImage/Dist/x32/FreeImage.h"
#include <string>

// Wrapper for FIBITMAP to ensure proper destruction
class FreeImageBmp
{
public:
	//Takes ownership of fiBitmap
	//FreeImageBmp(FIBITMAP* fiBitmap);
	FreeImageBmp(int width, int height, int bpp);
	FreeImageBmp(FREE_IMAGE_FORMAT imageType, FIMEMORY* fiMemory);
	FreeImageBmp(FREE_IMAGE_FORMAT imageFormat, const std::string& filename);
	~FreeImageBmp();

	FIBITMAP* const fiBitmap;
};
