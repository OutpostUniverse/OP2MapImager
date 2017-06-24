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

void FormatRenderFilename(string& renderFilenameOut, const string& filename, const string& destDirectory, ImageFormat imageFormat, bool overwrite)
{
	renderFilenameOut = XFile::AppendSubDirectory(filename, destDirectory);
	renderFilenameOut = XFile::ChangeFileExtension(renderFilenameOut, GetImageFormatExtension(imageFormat));

	if (!overwrite)
		renderFilenameOut = CreateUniqueFilename(renderFilenameOut);
}

bool ImageMap(const string& filename, int scaleFactor, ImageFormat imageFormat, string& renderFilenameOut, const string& destDirectory, bool overwrite = false)
{
	bool saveGame = false;
	if (XFile::ExtensionMatches(filename, ".OP2"))
		saveGame = true;

	MapData mapData(filename, saveGame);

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

	XFile::CreateDirectory(destDirectory);
	FormatRenderFilename(renderFilenameOut, filename, destDirectory, imageFormat, overwrite);

	bool saveSuccess = mapImager.SaveMapImage(renderFilenameOut, imageFormat);

	MapImager::DeInitialize();

	if (!saveSuccess)
		cerr << "Error encountered when attempting to save " + renderFilenameOut << endl;

	return saveSuccess;
}

void ConsoleImageMap(const string& filename, int scaleFactor, ImageFormat imageFormat, const string& destDirectory, bool quiet = false, bool overwrite = false)
{
	if (!quiet)
		cout << "Render initialized (May take up to 45 seconds): " + XFile::GetFilename(filename) << endl;

	string renderFilename;
	bool saveSuccess = ImageMap(filename, scaleFactor, imageFormat, renderFilename, destDirectory, overwrite);

	if (saveSuccess && !quiet)
		cout << "Render Saved: " + renderFilename << endl << endl;
}

void ConsoleImageMaps(const string& path, int scaleFactor, ImageFormat imageFormat, const string& destDirectory, bool quiet = false, bool overwrite = false)
{
	vector<string> filenames;
	XFile::GetFilesFromDirectory(filenames, path, ".map");
	XFile::GetFilesFromDirectory(filenames, path, ".OP2");

	if (filenames.size() == 0)
		throw exception("No map file found in the supplied directory.");

	if (!quiet)
		cout << filenames.size() + "files found for rendering.";

	for (auto& filename : filenames)
		ConsoleImageMap(filename, scaleFactor, imageFormat, destDirectory, quiet, overwrite);
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

struct ConsoleArgs
{
	bool helpRequested = false;
	bool overwrite = false;
	bool quiet = false;
	ImageFormat imageFormat = ImageFormat::PNG;
	int scaleFactor = 8;
	vector<string> paths;
	string destinationDirectory = "MapRenders";
};

void SortArguments(ConsoleArgs& consoleArgs,  int argc, char **argv)
{
	if (argc < 2)
	{
		consoleArgs.helpRequested = true;
		return;
	}

	for (int i = 1; i < argc; ++i)
	{
		string argument = ConvertToUpper(argv[i]);
		if (argument == "-Q" || argument == "--QUIET")
		{
			consoleArgs.quiet = true;
			i++;
		}
		else if (argument == "-O" || argument == "--OVERWRITE")
		{
			consoleArgs.overwrite = true;
			i++;
		}
		else if (argument == "-S" || argument == "--SCALE")
		{
			consoleArgs.scaleFactor = ParsePercentScaled(string(argv[i + 1]));
			i++;
		}
		else if (argument == "-D" || argument == "--DESTINATION")
		{
			consoleArgs.destinationDirectory = argv[i + 1];
			i++;
		}
		else if (argument == "-I" || argument == "--IMAGE")
		{
			consoleArgs.imageFormat = ParseImageType(argv[i + 1]);
			i++;
		}
		else if (argument == "-H" || argument == "--HELP")
		{
			consoleArgs.helpRequested = true;
			return;
		}
		else
			consoleArgs.paths.push_back(argv[i]);
	}
}

// argv[1] = path, argv[2] = Image Format (Default is PNG)
int main(int argc, char **argv)
{
	try
	{
		ConsoleArgs consoleArgs;
		SortArguments(consoleArgs, argc, argv);

		if (consoleArgs.helpRequested)
		{
			OutputHelp();
			return 0;
		}

		for each (string path in consoleArgs.paths)
		{
			if (XFile::IsDirectory(path))
				ConsoleImageMaps(path, consoleArgs.scaleFactor, consoleArgs.imageFormat, consoleArgs.destinationDirectory, consoleArgs.quiet, consoleArgs.overwrite);
			else if (IsMapOrSaveFileExtension(path))
				ConsoleImageMap(path, consoleArgs.scaleFactor, consoleArgs.imageFormat, consoleArgs.destinationDirectory, consoleArgs.quiet, consoleArgs.overwrite);
			else
				throw exception("You must provide either a directory or a file of type [.map|.OP2].");
		}
	}
	catch (exception e) {
		cerr << e.what() << endl;
		cerr << "Run without arguments to see usage message" << endl;
		return 1;
	}

	return 0;
}