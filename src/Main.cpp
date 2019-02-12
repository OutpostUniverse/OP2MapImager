#include "ConsoleArgumentParser.h"
#include "OP2Utility.h"
#include "MapImager.h"
#include <string>
#include <iostream>
#include <stdexcept>
#include <cstddef>
#include "Timer.h"

using namespace std;

// In order to use specific c functions in VolDecompress code:
//  * Compiler Warning 4996 has been diasabled: https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-3-c4996
//  * SDL checks have been set to NO/FALSE. (Security Development Lifecycle).

static const std::string consoleLineBreak("--------------------------------------------------");
static const std::string version = "2.0.1";

void OutputHelp();
void ExecuteCommand(const ConsoleArgs& consoleArgs);
void ImageMapFromConsole(const string& mapFilename, const string& resourceDirectory, const RenderSettings& renderSettings);
void ImageMapsInDirectoryFromConsole(const string& directory, RenderSettings renderSettings);
bool IsRenderableFileExtension(const string& filename);

int main(int argc, char **argv)
{
	try
	{
		//Timer timer;
		//timer.StartTimer();

		ConsoleArgumentParser argumentParser;
		ConsoleArgs consoleArgs = argumentParser.SortArguments(argc, argv);
		ExecuteCommand(consoleArgs);

		//if (!consoleArgs.renderSettings.quiet)
		//    cout << "Map Renders completed in " << timer.GetElapsedTime() << " seconds.";
	}
	catch (const std::exception& e) {
		cerr << e.what() << endl;
		cerr << "Run without arguments to see usage message." << endl << endl;

		return 1;
	}

	return 0;
}

void ExecuteCommand(const ConsoleArgs& consoleArgs)
{
	if (consoleArgs.renderSettings.helpRequested)
	{
		OutputHelp();
		return;
	}

	if (consoleArgs.paths.empty()) {
		throw runtime_error("You must provide at least one file or directory. To provide the current directory, enter './'.");
	}

	for (const auto& path : consoleArgs.paths)
	{
		if (XFile::IsDirectory(path)) {
			ImageMapsInDirectoryFromConsole(path, consoleArgs.renderSettings);
		}
		else if (IsRenderableFileExtension(path)) {
			ImageMapFromConsole(XFile::GetFilename(path), XFile::GetDirectory(path), consoleArgs.renderSettings);
		}
		else {
			throw runtime_error("You must provide either a directory or a file of type (.map|.OP2).");
		}
	}
}

// @param resourceDirectory: Directory containing archives and tilesets
void ImageMapFromConsole(const string& mapFilename, const string& resourceDirectory, const RenderSettings& renderSettings)
{
	if (!renderSettings.quiet) {
		cout << "Render initialized (May take up to 45 seconds): " + mapFilename << endl;
	}

	MapImager mapImager(resourceDirectory);
	string renderFilename;
	
	try {
		mapImager.ImageMap(renderFilename, mapFilename, renderSettings);

		if (!renderSettings.quiet) {
			cout << "Render Saved: " + renderFilename << endl << endl;
		}
	}
	catch (const std::exception& e) {
		cout << e.what() << endl << endl;
	}
}

void ImageMapsInDirectoryFromConsole(const string& directory, RenderSettings renderSettings)
{
	ResourceManager resourceManager(directory);

	vector<string> filenames = resourceManager.GetAllFilenamesOfType(".map", renderSettings.accessArchives);
	vector<string> saveFilenames = resourceManager.GetAllFilenames(R"(.*SGAME[0-9]\.OP2)"); //Regex
	
	filenames.insert(std::end(filenames), std::begin(saveFilenames), std::end(saveFilenames));

	// Loop starts at index size - 1 and ends after index 0 executes
	for (std::size_t i = filenames.size(); i-- > 0; )
	{
		if (filenames[i] == "wellpallet.map") {
			filenames.erase(filenames.begin() + i);
		}
	}

	if (filenames.size() == 0) {
		throw runtime_error("No map file or save file found in the supplied directory.");
	}

	if (!renderSettings.quiet)
	{
		cout << "File count found for rendering: " << to_string(filenames.size()) << endl;
		cout << consoleLineBreak << endl << endl;
	}

	for (const auto& filename : filenames) {
		ImageMapFromConsole(filename, directory, renderSettings);
	}

	if (!renderSettings.quiet)
	{
		cout << "Renders Complete!" << endl;
		cout << consoleLineBreak << endl << endl;
	}
}

bool IsRenderableFileExtension(const std::string& filename)
{
	return XFile::ExtensionMatches(filename, "MAP") || XFile::ExtensionMatches(filename, "OP2");
}

void OutputHelp()
{
	cout << endl;
	cout << "OP2MapImager Ver " << version << " - Outpost 2 Map and Saved Game Imager" << endl;
	cout << "Developed by Hooman and Brett208 (Vagabond)" << endl;
	cout << endl;
	cout << "+++ USAGE NOTES +++" << endl;
	cout << "  * Capable of saving multiple map files and entire directories." << endl;
	cout << "  * Scale Factor determines the final render size and represents the final pixel length of a single tile" << endl;
	cout << "    * A value of 32 renders at full size, or 32 pixels per tile." << endl;
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
