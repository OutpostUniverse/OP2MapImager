#include "OP2Utility.h"
#include "MapImager.h"
#include <string>
#include <iostream>

using namespace std;

void ImageMap(const string& filename, int percentScaled, ImageFormat imageFormat)
{
	MapData mapData(filename);

	MapImager::Initialize();

	int tileScaledWidth = 32 * percentScaled / 100;

	MapImager mapImager(
		mapData.mapHeader.MapTileWidth() * tileScaledWidth, 
		mapData.mapHeader.mapTileHeight * tileScaledWidth, 8, 32);


	for (size_t i = 0; i < mapData.tileSetSources.size(); ++i)
	{
		int flag = 0; //Optional load flag constant for FreeImage
		string filename = mapData.tileSetSources[i].GetTileSetFilename() + ".bmp";

		//freeImageHelper.AddTileSetRawBits();

		// Returns null if unsuccessful.
		//tileSetImages[i] = FreeImage_Load(FREE_IMAGE_FORMAT::FIF_BMP, filename.c_str(), flag);

	}

	cout << mapData.GetTileIndex(0, 0) << endl;
	cout << mapData.GetTileIndex(1, 0) << endl;
	cout << mapData.GetTileIndex(0, 1) << endl;
	cout << mapData.GetTileIndex(32, 0) << endl;

	for (unsigned int y = 0; y < mapData.mapHeader.mapTileHeight; y++)
	{
		for (unsigned int x = 0; x < mapData.mapHeader.MapTileWidth(); x++)
		{
			Rectangle sourceRect(0, mapData.GetImageIndex(x, y), tileScaledWidth, tileScaledWidth);

			//mapImager.ScaleAndPaste(mapData.GetTileSetIndex(x, y), sourceRect, 
			//	tileScaledWidth * x, tileScaledWidth * y, tileScaledWidth, tileScaledWidth);
		}
	}

	string imageFilename = mapData.tileSetSources[0].GetTileSetFilename() + ".png";
	bool imageSaveSuccess = mapImager.SaveMapImage(imageFilename, imageFormat);

	if (imageSaveSuccess)
		cout << "Map Image Saved as: " + imageFilename << endl;
	else
		cout << "Error encountered when attempting to save " + imageFilename << endl;

	MapImager::DeInitialize();
}

void ImageMaps(const string& path, int percentScaled, ImageFormat imageFormat)
{


	throw exception("Imaging multiple maps is not yet implemented.");
}

bool IsMapOrSaveFileExtension(const std::string& filename)
{
	return XFile::ExtensionMatches(filename, "MAP") || XFile::ExtensionMatches(filename, "OP2");
}

ImageFormat ParseImageType(const std::string& imageTypeString)
{
	string imageTypeStringUpper = XFile::ConvertToUpper(imageTypeString);

	if (imageTypeStringUpper == "PNG")
		return ImageFormat::PNG;

	if (imageTypeStringUpper == "JPG" || imageTypeStringUpper == "JPEG")
		return ImageFormat::JPG;

	if (imageTypeStringUpper == "BMP" || imageTypeStringUpper == "BITMAP")
		return ImageFormat::BMP;

	throw exception("Unable to determine final render file type. Try PNG, JPG, or BMP.");
}

int ParsePercentScaled(string& percentScaledStr)
{
	if (percentScaledStr[percentScaledStr.length() - 1] == '%')
		percentScaledStr.pop_back();

	// stoi will throw an exception if it is unable to parse the string into an integer
	int percentScaled = stoi(percentScaledStr);

	if (percentScaled <= 0)
		throw exception("Percent Scaled was set improperly.");

	return percentScaled;
}

void OutputHelp()
{
	cout << endl;
	cout << "OP2MapImager - The Outpost 2 Map Imager" << endl;
	cout << "Developed by Hooman and Brett208 (Vagabond)" << endl;
	cout << endl;
	cout << "The OP2MapImager executable must be placed in the same directory as the map's OP2 Tile Set Bitmaps." << endl;
	cout << endl;
	cout << "To save a map render:" << endl;
	cout << "    OP2MapImager [OutputFileType = PNG [PNG|JPG|BMP]] [PercentScaled = 25[%]] <mapFilename.[map|OP2]>" << endl;
	cout << "Example:" << endl;
	cout << "    OP2MapImager PNG 25 Ashes.map" << endl;
	cout << endl;
	cout << "To save an image of all maps in a directory:" << endl;
	cout << "OP2MapImager [Directory]" << endl;
	cout << endl;
	cout << "The default image type is PNG." << endl;
	cout << endl;
}

// argv[1] = path, argv[2] = Image Format (Default is PNG)
int main(int argc, char **argv)
{
	string formatStr = "PNG";
	string percentScaledStr = "25";
	string mapFilename = argv[argc - 1];
	
	// Determine command line instruction.
	if (argc < 4) {
		OutputHelp();
		return 1;
	}

	// TODO: Consider allowing optional parameter parsing for image render type and image percent scale
	formatStr = argv[2];
	percentScaledStr = argv[3];

	try {
		ImageFormat imageFormat = ParseImageType(formatStr);
		int percentScaled = ParsePercentScaled(percentScaledStr);

		if (XFile::IsDirectory(mapFilename))
			ImageMaps(mapFilename, percentScaled, imageFormat);
		else if (IsMapOrSaveFileExtension(mapFilename))
			ImageMap(mapFilename, percentScaled, imageFormat);
		
		throw exception("You must provide either a directory or a file of type [.map|.OP2].");
	}
	catch (exception e) {
		cerr << e.what() << endl;
		cerr << "Run without arguments to see usage message" << endl;
		return 1;
	}

	return 0;
}