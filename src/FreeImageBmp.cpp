#include "FreeImageBmp.h"
#include <stdexcept>

//FreeImageBmp::FreeImageBmp(FIBITMAP* fiBitmap) : fiBitmap(fiBitmap) {}

FreeImageBmp::FreeImageBmp(int width, int height, int bpp) : 
	fiBitmap(FreeImage_Allocate(width, height, bpp)) {}

FreeImageBmp::FreeImageBmp(FREE_IMAGE_FORMAT imageType, FIMEMORY* fiMemory) : 
	fiBitmap(FreeImage_LoadFromMemory(imageType, fiMemory, 0)) {}

FreeImageBmp::FreeImageBmp(FREE_IMAGE_FORMAT imageFormat, const std::string& filename) :
	fiBitmap(FreeImage_Load(imageFormat, filename.c_str())) {}

FreeImageBmp::~FreeImageBmp()
{
	if (fiBitmap != nullptr) {
		FreeImage_Unload(fiBitmap);
	}
}
