#include "ConsoleArgumentParser.h"
#include "OP2Utility/OP2Utility.h"
#include "MapImager.h"
#include <string>
#include <iostream>
#include <stdexcept>

using namespace std;

// In order to use specific c functions in VolDecompress code:
//  * Compiler Warning 4996 has been diasabled: https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-3-c4996
//  * SDL checks have been set to NO/FALSE. (Security Development Lifecycle).

static string consoleLineBreak("--------------------------------------------------");
static std::string version = "2.0.0";

void outputHelp();
void executeCommand(const ConsoleArgs& consoleArgs);
void imageMapFromConsole(const string& mapFilename, const RenderSettings& renderSettings);
void imageMapsInDirectoryFromConsole(const string& directory, RenderSettings renderSettings);
bool isRenderableFileExtension(const string& filename);
void debugPause();

int main(int argc, char **argv)
{
	try
	{
		ConsoleArgumentParser argumentParser;
		ConsoleArgs consoleArgs = argumentParser.sortArguments(argc, argv);

		executeCommand(consoleArgs);
	}
	catch (exception e) {
		cerr << e.what() << endl;
		cerr << "Run without arguments to see usage message." << endl << endl;

		debugPause();

		return 1;
	}

	debugPause();

	return 0;
}

void executeCommand(const ConsoleArgs& consoleArgs)
{
	if (consoleArgs.renderSettings.helpRequested)
	{
		outputHelp();
		return;
	}

	if (consoleArgs.paths.size() == 0)
		throw runtime_error("You must provide at least one file or directory. To provide the current directory, enter './'.");

	for (string path : consoleArgs.paths)
	{
		if (XFile::isDirectory(path))
			imageMapsInDirectoryFromConsole(path, consoleArgs.renderSettings);
		else if (isRenderableFileExtension(path))
			imageMapFromConsole(path, consoleArgs.renderSettings);
		else
			throw runtime_error("You must provide either a directory or a file of type (.map|.OP2).");
	}
}

void imageMapFromConsole(const string& mapFilename, const RenderSettings& renderSettings)
{
	if (!renderSettings.quiet)
		cout << "Render initialized (May take up to 45 seconds): " + XFile::getFilename(mapFilename) << endl;

	MapImager mapImager(XFile::removeFilename(mapFilename));
	string renderFilename;
	bool saveSuccess = mapImager.imageMap(renderFilename, mapFilename, renderSettings);

	if (saveSuccess && !renderSettings.quiet)
		cout << "Render Saved: " + renderFilename << endl << endl;
}

void imageMapsInDirectoryFromConsole(const string& directory, RenderSettings renderSettings)
{
	ResourceManager resourceManager(directory);

	vector<string> filenames = resourceManager.getAllFilenamesOfType(directory, ".map", renderSettings.accessArchives);
	vector<string> saveFilenames = resourceManager.getAllFilenames(directory, R"(.*SGAME[0-9]\.OP2)"); //Regex
	
	filenames.insert(std::end(filenames), std::begin(saveFilenames), std::end(saveFilenames));

	for (int i = filenames.size() - 1; i >= 0; i--) 
	{
		string filename = XFile::getFilename(filenames[i]);
		if (filename == "wellpallet.map")
			filenames.erase(filenames.begin() + i);
	}

	if (filenames.size() == 0)
		throw runtime_error("No map file or save file found in the supplied directory.");

	if (!renderSettings.quiet)
	{
		cout << "File count found for rendering: " << to_string(filenames.size()) << endl;
		cout << consoleLineBreak << endl << endl;
	}

	for (string filename : filenames)
		imageMapFromConsole(filename, renderSettings);

	if (!renderSettings.quiet)
	{
		cout << "Renders Complete!" << endl;
		cout << consoleLineBreak << endl << endl;
	}
}

bool isRenderableFileExtension(const std::string& filename)
{
	return XFile::extensionMatches(filename, "MAP") || XFile::extensionMatches(filename, "OP2");
}

void outputHelp()
{
	cout << endl;
	cout << "OP2MapImager Ver " << version << " - Outpost 2 Map and Saved Game Imager" << endl;
	cout << "Developed by Hooman and Brett208 (Vagabond)" << endl;
	cout << endl;
	cout << "+++ USAGE NOTES +++" << endl;
	cout << "  * Capable of saving multiple map files and entire directories." << endl;
	cout << "  * The OP2MapImager executable and FreeImage.dll must be in the same directory as the tileset BMPs." << endl;
	cout << "  * Scale Factor determines the final render size and represents the final pixel length of a single tile" << endl;
	cout << "    * Max Value: 32, renders at full size, or 32 pixels per tile." << endl;
	cout << "    * Min Value: 1, renders at 1 pixel per tile" << endl;
	cout << endl;
	cout << "+++ EXAMPLE COMMANDS +++" << endl;
	cout << "  * OP2MapImager mapFilename.[map|OP2]" << endl;
	cout << "  * OP2MapImager -s 16 -o -q Ashes.map eden01.map sgame0.op2" << endl;
	cout << "  * OP2MapImager --Scale 8 --ImageFormat BMP [Directory of choice]" << endl;
	cout << endl;
	cout << "+++ OPTIONAL ARGUMENTS +++" << endl;
	cout << "  -H / --Help / -?: Displays help information." << endl;
	cout << "  -Q / --Quiet: [Default false] Add switch to run application without issuing console messages." << endl;
	cout << "  -O / --Overwrite: [Default false] Add switch to allow application to overwrite existing files." << endl;
	cout << "  -D / --DestinationDirectory: [Default MapRenders]. Add switch and name of new destination path." << endl;
	cout << "  -I / --ImageFormat: [Default PNG]. Allows PNG|JPG|BMP. Sets the image format of the final render." << endl;
	cout << "  -S / --Scale: [Default 4] Sets Scale Factor of image." << endl;
	cout << "  -A / --AccessArchives [Default true]. Add switch to disable searching VOL archives for map and well files." << endl;
	cout << endl;
	cout << "For more information about Outpost 2 visit the Outpost Universe (http://outpost2.net/)." << endl;
	cout << "Image Manipulation accomplished through FreeImage (http://freeimage.sourceforge.net/)." << endl;
	cout << endl;
}

void debugPause()
{
#if defined _DEBUG
	getchar();
#endif
}