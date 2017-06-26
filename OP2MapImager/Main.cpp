#include "OP2Utility.h"
#include "MapImager.h"
#include <string>
#include <iostream>
#include <functional>

using namespace std;

struct RenderSettings
{
	ImageFormat imageFormat = ImageFormat::PNG;
	int scaleFactor = 4;
	string destDirectory = "MapRenders";
	bool overwrite = false;
	bool quiet = false;
	bool helpRequested = false;
};

struct ConsoleArgs
{
	RenderSettings renderSettings;
	vector<string> paths;
};

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

void loadTileSets(MapData& mapData, MapImager& mapImager)
{
	for (size_t i = 0; i < mapData.tileSetSources.size(); ++i)
	{
		if (mapData.tileSetSources[i].numTiles == 0)
			continue;

		string filename = mapData.tileSetSources[i].GetTileSetFilename() + ".bmp";

		mapImager.AddTileSet(filename, ImageFormat::BMP);

		//TODO: Load Bmps into memory and set in mapImager if Outpost2 specific BMP file.
		//mapImager.AddTileSetRawBits()
	}
}

void setRenderTiles(MapData& mapData, MapImager& mapImager)
{
	for (unsigned int y = 0; y < mapData.mapHeader.mapTileHeight; y++)
		for (unsigned int x = 0; x < mapData.mapHeader.MapTileWidth(); x++)
			mapImager.PasteTile(mapData.GetTileSetIndex(x, y), mapData.GetImageIndex(x, y), x, y);
}

bool ImageMap(string& renderFilenameOut, const string& filename, const RenderSettings& renderSettings)
{
	bool saveGame = false;
	if (XFile::ExtensionMatches(filename, ".OP2"))
		saveGame = true;

	MapData mapData(filename, saveGame);

	MapImager::Initialize();

	MapImager mapImager(
		mapData.mapHeader.MapTileWidth(),
		mapData.mapHeader.mapTileHeight, 24, renderSettings.scaleFactor);

	loadTileSets(mapData, mapImager);
	setRenderTiles(mapData, mapImager);

	XFile::CreateDirectory(renderSettings.destDirectory);
	FormatRenderFilename(renderFilenameOut, filename, renderSettings.destDirectory, renderSettings.imageFormat, renderSettings.overwrite);

	bool saveSuccess = mapImager.SaveMapImage(renderFilenameOut, renderSettings.imageFormat);

	MapImager::DeInitialize();

	if (!saveSuccess)
		cerr << "Error encountered when attempting to save " + renderFilenameOut << endl;

	return saveSuccess;
}

void ConsoleImageMap(const string& mapFilename, const RenderSettings& renderSettings)
{
	if (!renderSettings.quiet)
		cout << "Render initialized (May take up to 45 seconds): " + XFile::GetFilename(mapFilename) << endl;

	string renderFilename;
	bool saveSuccess = ImageMap(renderFilename, mapFilename, renderSettings);

	if (saveSuccess && !renderSettings.quiet)
		cout << "Render Saved: " + renderFilename << endl << endl;
}

void ConsoleImageMapsInDirectory(const string& directory, RenderSettings renderSettings)
{
	vector<string> filenames;
	XFile::GetFilesFromDirectory(filenames, directory, ".map");
	XFile::GetFilesFromDirectory(filenames, directory, ".OP2");

	if (filenames.size() == 0)
		throw exception("No map file found in the supplied directory.");

	if (!renderSettings.quiet)
		cout << filenames.size() + "files found for rendering.";

	for (auto& filename : filenames)
		ConsoleImageMap(filename, renderSettings);
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

ImageFormat ParseImageTypeToEnum(const std::string& imageTypeString)
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

int ParsePercentScaled(const string& percentScaledStr)
{
	// stoi will throw an exception if it is unable to parse the string into an integer
	int percentScaled = stoi(percentScaledStr);

	if (percentScaled <= 0)
		throw exception("Scale Factor was set improperly.");

	return percentScaled;
}

void OutputHelp()
{
	cout << endl;
	cout << "OP2MapImager Ver 1.0 - Outpost 2 Map and Saved Game Imager" << endl;
	cout << "Developed by Hooman and Brett208 (Vagabond)" << endl;
	cout << endl;
	cout << "+++ USAGE NOTES +++" << endl;
	cout << "  * Capable of saving multiple map files and entire directories." << endl;
	cout << "  * The OP2MapImager executable and FreeImage.DLL must be in the same directory as the tileset BMPs." << endl;
	cout << "  * Tileset BMPs and map files must be manually extracted from VOL files before use with OP2MapImager." << endl;
	cout << "  * Scale Factor determines the final render size and represents the final pixel length of a single tile" << endl;
	cout << "    * Max Value: 32, renders at full size, or 32 pixels per tile." << endl;
	cout << "    * Min Value: 1, renders at 1 pixel per tile" << endl;
	cout << endl;
	cout << "+++ EXAMPLE COMMANDS +++" << endl;
	cout << "  * OP2MapImager mapFilename.[map|OP2]" << endl;
	cout << "  * OP2MapImager -s 16 -o true -q true Ashes.map eden01.map sgame0.op2" << endl;
	cout << "  * OP2MapImager --Scale 8 --ImageFormat BMP [Directory of choice]" << endl;
	cout << endl;
	cout << "+++ OPTIONAL ARGUMENTS +++" << endl;
	cout << "  -H / --Help: Displays Help File" << endl;
	cout << "  -Q / --Quiet: [Defaul false] Makes application run without issueing console messages." << endl;
	cout << "  -O / --Overwrite: [Default false] Allows application to overwrite existing files when rendering new maps." << endl;
	cout << "  -D / --DestinationDirectory: " << endl;
	cout << "  -I / --ImageFormat: [Default PNG]. Allows PNG|JPG|BMP. Sets the image format of the final prameter." << endl;
	cout << "  -S / --Scale: [Default 4] Sets Scale Factor of image." << endl;
	cout << endl;
	cout << "For more information about Outpost 2 visit the Outpost Universe (http://outpost2.net/)" << endl;
	cout << "Image Manipulation accomplished through FreeImage (http://freeimage.sourceforge.net/)" << endl;
	cout << endl;
}

bool ParseBool(const string& str)
{
	string upperStr = ConvertToUpper(str);

	if (upperStr == "TRUE" || upperStr == "YES")
		return true;

	if (upperStr == "FALSE" || upperStr == "NO")
		return false;

	throw invalid_argument("Unable to parse argument into a valid boolean (True/False).");
}

void CheckForMissingSwitchArgument(int index, int argc, int numberOfArgsToPass)
{
	if (index + numberOfArgsToPass >= argc)
		throw exception("Missing the final argument for the supplied switch.");
}

void parseHelp(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.helpRequested = true;
}

void parseQuiet(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.quiet = ParseBool(value);
}

void parseOverwrite(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.overwrite = ParseBool(value);
}

void parseScale(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.scaleFactor = ParsePercentScaled(value);
}

void parseDestDirectory(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.destDirectory = value;
}

void parseImageFormat(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.imageFormat = ParseImageTypeToEnum(value);
}



struct ConsoleSwitch
{
	ConsoleSwitch() { }

	ConsoleSwitch(string shortSwitch, string longSwitch, function<void(const char* value, ConsoleArgs&)> parseFunction, int numberOfArgs)
	{
		this->shortSwitch = shortSwitch;
		this->longSwitch = longSwitch;
		this->parseFunction = parseFunction;
		this->numberOfArgs = numberOfArgs;
	}

	string shortSwitch;
	string longSwitch;
	function<void(const char*, ConsoleArgs&)> parseFunction;
	int numberOfArgs; // The switch statement does not count as an argument.

	bool argumentMatch(string argument)
	{
		return argument == shortSwitch || argument == longSwitch;
	}
};

ConsoleSwitch consoleSwitches[]
{
	ConsoleSwitch("-S", "--SCALE", parseScale, 1),
	ConsoleSwitch("-I", "--IMAGEFORMAT", parseImageFormat, 1),
	ConsoleSwitch("-D", "--DESTINATIONDIRECTORY", parseDestDirectory, 1),
	ConsoleSwitch("-H", "--HELP", parseHelp, 0),
	ConsoleSwitch("-Q", "--QUIET", parseQuiet, 1),
	ConsoleSwitch("-O", "--OVERWRITE", parseOverwrite, 1)
};

bool findSwitch(char* argumentChar, ConsoleSwitch& currentSwitch)
{
	string argument = ConvertToUpper(argumentChar);

	for each (ConsoleSwitch consoleSwitch in consoleSwitches)
	{
		if (consoleSwitch.argumentMatch(argument))
		{
			currentSwitch = consoleSwitch;
			return true;
		}
	}

	return false;
}

void SortArguments(ConsoleArgs& consoleArgs, int argc, char **argv)
{
	if (argc < 2)
	{
		consoleArgs.renderSettings.helpRequested = true;
		return;
	}

	ConsoleSwitch currentSwitch;

	for (int i = 1; i < argc; ++i)
	{
		bool switchFound = findSwitch(argv[i], currentSwitch);

		if (switchFound)
		{
			CheckForMissingSwitchArgument(i, argc, currentSwitch.numberOfArgs);

			if (currentSwitch.numberOfArgs == 0)
				currentSwitch.parseFunction(argv[i], consoleArgs);
			else
				currentSwitch.parseFunction(argv[i + 1], consoleArgs);

			i = i + currentSwitch.numberOfArgs;
		}
		else
		{
			consoleArgs.paths.push_back(argv[i]);
		}
	}
}

int main(int argc, char **argv)
{
	try
	{
		ConsoleArgs consoleArgs;
		SortArguments(consoleArgs, argc, argv);

		if (consoleArgs.renderSettings.helpRequested)
		{
			OutputHelp();
			return 0;
		}

		for each (string path in consoleArgs.paths)
		{
			if (XFile::IsDirectory(path))
				ConsoleImageMapsInDirectory(path, consoleArgs.renderSettings);
			else if (IsMapOrSaveFileExtension(path))
				ConsoleImageMap(path, consoleArgs.renderSettings);
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