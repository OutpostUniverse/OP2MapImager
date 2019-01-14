#include "ConsoleArgumentParser.h"
#include "OP2Utility.h"
#include <stdexcept>

using namespace std;

ConsoleArgumentParser::ConsoleArgumentParser()
{
	consoleSwitches.push_back(ConsoleSwitch("-S", "--SCALE", ParseScale, 1));
	consoleSwitches.push_back(ConsoleSwitch("-I", "--IMAGEFORMAT", ParseImageFormat, 1));
	consoleSwitches.push_back(ConsoleSwitch("-D", "--DESTINATIONDIRECTORY", ParseDestDirectory, 1));
	consoleSwitches.push_back(ConsoleSwitch("-H", "--HELP", ParseHelp, 0));
	consoleSwitches.push_back(ConsoleSwitch("-?", "--?", ParseHelp, 0));
	consoleSwitches.push_back(ConsoleSwitch("-Q", "--QUIET", ParseQuiet, 0));
	consoleSwitches.push_back(ConsoleSwitch("-O", "--OVERWRITE", ParseOverwrite, 0));
	consoleSwitches.push_back(ConsoleSwitch("-A", "--ACCESSARCHIVES", ParseAccessArchives, 0));
};

bool ConsoleArgumentParser::FindSwitch(char* argumentChar, ConsoleSwitch& currentSwitch)
{
	string argument = StringHelper::ConvertToUpper(argumentChar);

	// Remove trailing colon from switch statements.
	if (argument.size() > 0 && argument[argument.size() - 1] == ':') {
		argument.pop_back();
	}

	for (const auto& consoleSwitch : consoleSwitches)
	{
		if (consoleSwitch.ArgumentMatch(argument)) {
			currentSwitch = consoleSwitch;
			return true;
		}
	}

	return false;
}

ConsoleArgs ConsoleArgumentParser::SortArguments(int argc, char **argv)
{
	ConsoleArgs consoleArgs;

	if (IsTooFewArguments(argc)) {
		consoleArgs.renderSettings.helpRequested = true;
		return consoleArgs;
	}

	ConsoleSwitch currentSwitch;

	for (int i = 1; i < argc; ++i)
	{
		bool switchFound = FindSwitch(argv[i], currentSwitch);

		if (switchFound)
		{
			CheckForMissingSwitchArgument(i, argc, currentSwitch.numberOfArgs);

			if (currentSwitch.numberOfArgs == 0) {
				currentSwitch.parseFunction(argv[i], consoleArgs);
			}
			else {
				currentSwitch.parseFunction(argv[i + 1], consoleArgs);
			}
			i = i + currentSwitch.numberOfArgs;
		}
		else {
			consoleArgs.paths.push_back(argv[i]);
		}
	}

	return consoleArgs;
}

ImageFormat ConsoleArgumentParser::ParseImageTypeToEnum(const std::string& imageTypeString)
{
	string imageTypeStringUpper = StringHelper::ConvertToUpper(imageTypeString);

	if (imageTypeStringUpper == "PNG") {
		return ImageFormat::PNG;
	}
	if (imageTypeStringUpper == "JPG" || imageTypeStringUpper == "JPEG") {
		return ImageFormat::JPG;
	}
	if (imageTypeStringUpper == "BMP" || imageTypeStringUpper == "BITMAP") {
		return ImageFormat::BMP;
	}

	throw runtime_error("Unable to determine final render file type. Try PNG, JPG, or BMP.");
}

bool ConsoleArgumentParser::ParseBool(const string& str)
{
	string upperStr = StringHelper::ConvertToUpper(str);

	if (upperStr == "TRUE" || upperStr == "YES") {
		return true;
	}
	if (upperStr == "FALSE" || upperStr == "NO") {
		return false;
	}

	throw invalid_argument("Unable to parse argument into a valid boolean (True/False).");
}

void ConsoleArgumentParser::CheckForMissingSwitchArgument(int index, int argc, int numberOfArgsToPass)
{
	if (index + numberOfArgsToPass >= argc) {
		throw runtime_error("Missing the final argument for the supplied switch.");
	}
}

bool ConsoleArgumentParser::IsTooFewArguments(int argumentCount)
{
	return argumentCount < 2;
}

void ConsoleArgumentParser::ParseHelp(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.helpRequested = true;
}

void ConsoleArgumentParser::ParseQuiet(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.quiet = true;
}

void ConsoleArgumentParser::ParseOverwrite(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.overwrite = true;
}

void ConsoleArgumentParser::ParseAccessArchives(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.accessArchives = false;
}

void ConsoleArgumentParser::ParseScale(const char* value, ConsoleArgs& consoleArgs)
{
	// stoi will throw an exception if it is unable to parse the string into an integer
	int scaleFactor = stoi(value);

	if (scaleFactor <= 0) {
		throw runtime_error("Scale Factor was set improperly.");
	}

	consoleArgs.renderSettings.scaleFactor = scaleFactor;
}

void ConsoleArgumentParser::ParseDestDirectory(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.destDirectory = value;
}

void ConsoleArgumentParser::ParseImageFormat(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.imageFormat = ParseImageTypeToEnum(value);
}
