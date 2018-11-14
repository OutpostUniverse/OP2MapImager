#include "FreeImageBmp.h"
#include <stdexcept>

FreeImageBmp::FreeImageBmp(FIBITMAP* fiBitmap) : fiBitmap(fiBitmap)
{
	if (fiBitmap == nullptr) {
		throw std::runtime_error("Unable to create new FreeImage bitmap from null handle.");
	}
}

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

FreeImageBmp::~FreeImageBmp()
{
	FreeImage_Unload(fiBitmap);
}


unsigned FreeImageBmp::Width() const
{
	return FreeImage_GetWidth(fiBitmap);
}

unsigned FreeImageBmp::Height() const
{
	return FreeImage_GetHeight(fiBitmap);
}

FreeImageBmp FreeImageBmp::Rescale(int scaledWidth, int scaledHeight) const
{
	try {
		return FreeImageBmp(FreeImage_Rescale(fiBitmap, scaledWidth, scaledHeight));
	} catch(...) {
		// Upgrade exception to more detailed error message
		throw std::runtime_error(
			"Unable to create a new bitmap by scaling existing bitmap with requested scaled width: " +
			std::to_string(scaledWidth) + " , scaled height: " + std::to_string(scaledHeight));
	}
}

FreeImageBmp FreeImageBmp::CreateView(unsigned left, unsigned top, unsigned right, unsigned bottom) const
{
	return FreeImageBmp(FreeImage_CreateView(fiBitmap, left, top, right, bottom));
}

void FreeImageBmp::Paste(const FreeImageBmp& dest, int x, int y, int alpha) const
{
	if (!FreeImage_Paste(dest.fiBitmap, fiBitmap, x, y, alpha)) {
		throw std::runtime_error("Pasting of FreeImage bitmap failed");
	}
}

void FreeImageBmp::Save(const std::string& filename, FREE_IMAGE_FORMAT fiImageFormat, int flags) const
{
	if (!FreeImage_Save(fiImageFormat, fiBitmap, filename.c_str(), flags)) {
		throw std::runtime_error(
			"Error saving FreeImage bitmap to file: " + filename
		);
	}
}
