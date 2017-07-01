#include "ConsoleArgumentParser.h"
#include "OP2Utility.h"
#include "MapImager.h"
#include <string>
#include <iostream>

using namespace ConsoleArgumentParser;

// In order to use specific c functions in VolDecompress code:
//  * Compiler Warning 4996 has been diasabled: https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-3-c4996
//  * SDL checks have been set to NO/FALSE. (Security Development Lifecycle).

MapImager mapImager;

void outputHelp();
void imageMapFromConsole(const string& mapFilename, const RenderSettings& renderSettings);
void imageMapsInDirectoryFromConsole(const string& directory, RenderSettings renderSettings);
bool isRenderableFileExtension(const std::string& filename);

int main(int argc, char **argv)
{
	try
	{
		ConsoleArgs consoleArgs;
		SortArguments(consoleArgs, argc, argv);

		if (consoleArgs.renderSettings.helpRequested)
		{
			outputHelp();
			return 0;
		}

		if (consoleArgs.paths.size() == 0)
			throw exception("You must provide at least one file or directory. To provide the current directory, enter '/'.");

		for each (string path in consoleArgs.paths)
		{
			if (XFile::isDirectory(path))
				imageMapsInDirectoryFromConsole(path, consoleArgs.renderSettings);
			else if (isRenderableFileExtension(path))
				imageMapFromConsole(path, consoleArgs.renderSettings);
			else
				throw exception("You must provide either a directory or a file of type [.map|.OP2].");
		}
	}
	catch (exception e) {
		cerr << e.what() << endl;
		cerr << "Run without arguments to see usage message." << endl;
		return 1;
	}

	return 0;
}

void imageMapFromConsole(const string& mapFilename, const RenderSettings& renderSettings)
{
	if (!renderSettings.quiet)
		cout << "Render initialized (May take up to 45 seconds): " + XFile::getFilename(mapFilename) << endl;

	string renderFilename;
	bool saveSuccess = mapImager.imageMap(renderFilename, mapFilename, renderSettings);

	if (saveSuccess && !renderSettings.quiet)
		cout << "Render Saved: " + renderFilename << endl << endl;
}

void imageMapsInDirectoryFromConsole(const string& directory, RenderSettings renderSettings)
{
	ArchiveHelper::extractAllOfFileType(directory, ".map");

	vector<string> filenames;
	XFile::getFilesFromDirectory(filenames, directory, ".map");
	XFile::getFilesFromDirectory(filenames, directory, ".OP2");

	if (filenames.size() == 0)
		throw exception("No map file or save file found in the supplied directory.");

	if (!renderSettings.quiet)
		cout << "File count found for rendering: " << to_string(filenames.size()) << endl << endl;

	for (auto& filename : filenames)
		imageMapFromConsole(filename, renderSettings);
}

bool isRenderableFileExtension(const std::string& filename)
{
	return XFile::extensionMatches(filename, "MAP") || XFile::extensionMatches(filename, "OP2");
}

void outputHelp()
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
	cout << "  -Q / --Quiet: [Defaul false] Add switch to run application without issuing console messages." << endl;
	cout << "  -O / --Overwrite: [Default false] Add switch to allow application to overwrite existing files." << endl;
	cout << "  -D / --DestinationDirectory: [Default MapRenders]. Add switch and name name of new path." << endl;
	cout << "  -I / --ImageFormat: [Default PNG]. Allows PNG|JPG|BMP. Sets the image format of the final prameter." << endl;
	cout << "  -S / --Scale: [Default 4] Sets Scale Factor of image." << endl;
	cout << "  -A / --AccessArchives [Default true]. Add switch to disable searching VOL archives for map and well files." << endl;
	cout << endl;
	cout << "For more information about Outpost 2 visit the Outpost Universe (http://outpost2.net/)" << endl;
	cout << "Image Manipulation accomplished through FreeImage (http://freeimage.sourceforge.net/)" << endl;
	cout << endl;
}