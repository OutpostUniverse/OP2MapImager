#include "OP2Utility.h"
#include "MapImager.h"
#include <string>
#include <iostream>

using namespace std;

std::string GetImageFormatExtension(ImageFormat imageFormat)
{
	switch (imageFormat)
	{
	case ImageFormat::PNG:
		return ".png";
	case ImageFormat::BMP:
		return ".bmp";
	case ImageFormat::JPG:
		return ".jpg";
	default:
		return ".bmp";
	}
}

string CreateUniqueFilename(const string& filename)
{
	string uniqueFilename = filename;
	
	int pathIndex = 1;
	while (XFile::PathExists(uniqueFilename))
	{
		uniqueFilename = XFile::AppendToFilename(filename, "_" + std::to_string(pathIndex));
		pathIndex++;

		if (pathIndex >= std::numeric_limits<int>::max())
			throw std::exception("Too many files with the same filename.");
	}

	return uniqueFilename;
}

bool ImageMap(const string& filename, int scaleFactor, ImageFormat imageFormat, bool overwrite = false)
{
	MapData mapData(filename);

	MapImager::Initialize();

	MapImager mapImager(
		mapData.mapHeader.MapTileWidth(),
		mapData.mapHeader.mapTileHeight, 24, scaleFactor);

	for (size_t i = 0; i < mapData.tileSetSources.size(); ++i)
	{
		if (mapData.tileSetSources[i].numTiles == 0)
			continue;

		string filename = mapData.tileSetSources[i].GetTileSetFilename() + ".bmp";

		mapImager.AddTileSet(filename, ImageFormat::BMP);

		//TODO: Load Bmps into memory and set in mapImager if Outpost2 specific BMP file.
		//mapImager.AddTileSetRawBits()
	}

	for (unsigned int y = 0; y < mapData.mapHeader.mapTileHeight; y++)
		for (unsigned int x = 0; x < mapData.mapHeader.MapTileWidth(); x++)
			mapImager.PasteTile(mapData.GetTileSetIndex(x, y), mapData.GetImageIndex(x, y), x, y);

	string destDirectory = "MapRenders";
	XFile::CreateDirectory(destDirectory);
	string imageFilename = XFile::AppendSubDirectory(filename, destDirectory);
	imageFilename = XFile::ChangeFileExtension(imageFilename, GetImageFormatExtension(imageFormat));

	if (!overwrite)
		imageFilename = CreateUniqueFilename(imageFilename);

	bool saveSuccess = mapImager.SaveMapImage(imageFilename, imageFormat);

	MapImager::DeInitialize();

	if (!saveSuccess)
		cerr << "Error encountered when attempting to save " + imageFilename << endl;

	return saveSuccess;
}

void ConsoleImageMap(const string& filename, int scaleFactor, ImageFormat imageFormat, bool quiet = false, bool overwrite = false)
{
	if (!quiet)
		cout << "Render initialized (May take up to 45 seconds): " + XFile::GetFilename(filename) << endl;

	bool saveSuccess = ImageMap(filename, scaleFactor, imageFormat);

	if (saveSuccess && !quiet)
		cout << "Render Saved: " + filename << endl << endl;
}

void ConsoleImageMaps(const string& path, int scaleFactor, ImageFormat imageFormat, bool quiet = false, bool overwrite = false)
{
	vector<string> filenames;
	XFile::GetFilesFromDirectory(filenames, path, ".map");

	if (filenames.size() == 0)
		throw exception("No map file found in the supplied directory.");

	if (!quiet)
		cout << filenames.size() + "files found for rendering.";

	for (auto& filename : filenames)
		ConsoleImageMap(filename, scaleFactor, imageFormat, quiet, overwrite);
}

bool IsMapOrSaveFileExtension(const std::string& filename)
{
	return XFile::ExtensionMatches(filename, "MAP") || XFile::ExtensionMatches(filename, "OP2");
}

void ConvertToUpper(string& str)
{
	for (auto & c : str) c = toupper(c);
}

string ConvertToUpper(const string& str)
{
	string newString = str;
	ConvertToUpper(newString);

	return newString;
}

ImageFormat ParseImageType(const std::string& imageTypeString)
{
	string imageTypeStringUpper = ConvertToUpper(imageTypeString);

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
	cout << "    OP2MapImager [OutputFileType = PNG [PNG|JPG|BMP]] [ScaleFactor = 8] <mapFilename.[map|OP2]>" << endl;
	cout << "Example:" << endl;
	cout << "    OP2MapImager PNG 16 Ashes.map" << endl;
	cout << endl;
	cout << "To save an image of all maps in a directory:" << endl;
	cout << "OP2MapImager [Directory]" << endl;
	cout << endl;
	cout << "The default image type is PNG." << endl;
	cout << "ScaleFactor represents the pixel length of a single tile on the final render." << endl;
	cout << "    Max Value: 32, renders at 100%, or 32 pixels per tile. 1 = render at 1 pixel per tile" << endl;
	cout << "    Min Value: 1, renders at 1 pixel per tile" << endl;
	cout << endl;
}

// argv[1] = path, argv[2] = Image Format (Default is PNG)
int main(int argc, char **argv)
{
	string formatStr = "PNG";
	string scaleFactorStr = "8";
	string mapFilename = argv[argc - 1];
	
	// Determine command line instruction.
	if (argc < 4) {
		OutputHelp();
		return 1;
	}

	// TODO: Consider allowing optional parameter parsing for image render type and image percent scale
	formatStr = argv[1];
	scaleFactorStr = argv[2];

	bool overwrite = false;
	bool quiet = false;

	try {
		ImageFormat imageFormat = ParseImageType(formatStr);
		int scaleFactor = ParsePercentScaled(scaleFactorStr);

		if (XFile::IsDirectory(mapFilename))
			ConsoleImageMaps(mapFilename, scaleFactor, imageFormat);
		else if (IsMapOrSaveFileExtension(mapFilename))
			ConsoleImageMap(mapFilename, scaleFactor, imageFormat, quiet, overwrite);
		else
			throw exception("You must provide either a directory or a file of type [.map|.OP2].");
	}
	catch (exception e) {
		cerr << e.what() << endl;
		cerr << "Run without arguments to see usage message" << endl;
		return 1;
	}

	return 0;
}