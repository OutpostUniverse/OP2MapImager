#include "FreeImageBmp.h"
#include <stdexcept>

FreeImageBmp::FreeImageBmp(FreeImageBmp&& other) : fiBitmap(other.fiBitmap)
{
	other.fiBitmap = nullptr;
}

FreeImageBmp::FreeImageBmp(int width, int height, int bpp) : 
	fiBitmap(FreeImage_Allocate(width, height, bpp)) 
{
	if (fiBitmap == nullptr) {
		throw std::runtime_error("Unable to create a default bitmap with the following properties, Width: " + std::to_string(width) +
			" , Height: " + std::to_string(height) + " , Bits per pixel: " + std::to_string(bpp));
	}
}

FreeImageBmp::FreeImageBmp(FREE_IMAGE_FORMAT imageType, FIMEMORY* fiMemory) : 
	fiBitmap(FreeImage_LoadFromMemory(imageType, fiMemory, 0)) 
{
	if (fiBitmap == nullptr) {
		throw std::runtime_error("Unable to load a bitmap from provided location in memory");
	}
}

FreeImageBmp::FreeImageBmp(FREE_IMAGE_FORMAT imageFormat, const std::string& filename) :
	fiBitmap(FreeImage_Load(imageFormat, filename.c_str())) 
{
	if (fiBitmap == nullptr) {
		throw std::runtime_error("Unable to load bitmap located at " + filename);
	}
}

FreeImageBmp::FreeImageBmp(const FreeImageBmp& freeImageBmp, int scaledWidth, int scaledHeight) :
	fiBitmap(FreeImage_Rescale(freeImageBmp.fiBitmap, scaledWidth, scaledHeight))
{
	if (fiBitmap == nullptr) {
		throw std::runtime_error(
			"Unable to create a new bitmap by scaling existing bitmap with requested scaled width: " + 
			std::to_string(scaledWidth) + " , scaled height: " + std::to_string(scaledHeight));
	}
}

FreeImageBmp::~FreeImageBmp()
{
	FreeImage_Unload(fiBitmap);
}
