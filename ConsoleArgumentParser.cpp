#include "ConsoleArgumentParser.h"
#include "OP2Utility/StringHelper.h"
#include <stdexcept>

using namespace std;

ConsoleArgumentParser::ConsoleArgumentParser()
{
	consoleSwitches.push_back(ConsoleSwitch("-S", "--SCALE", parseScale, 1));
	consoleSwitches.push_back(ConsoleSwitch("-I", "--IMAGEFORMAT", parseImageFormat, 1));
	consoleSwitches.push_back(ConsoleSwitch("-D", "--DESTINATIONDIRECTORY", parseDestDirectory, 1));
	consoleSwitches.push_back(ConsoleSwitch("-H", "--HELP", parseHelp, 0));
	consoleSwitches.push_back(ConsoleSwitch("-?", "--?", parseHelp, 0));
	consoleSwitches.push_back(ConsoleSwitch("-Q", "--QUIET", parseQuiet, 0));
	consoleSwitches.push_back(ConsoleSwitch("-O", "--OVERWRITE", parseOverwrite, 0));
	consoleSwitches.push_back(ConsoleSwitch("-A", "--ACCESSARCHIVES", parseAccessArchives, 0));
};

bool ConsoleArgumentParser::findSwitch(char* argumentChar, ConsoleSwitch& currentSwitch)
{
	string argument = StringHelper::convertToUpper(argumentChar);

	// Remove trailing colon from switch statements.
	if (argument.size() > 0 && argument[argument.size() - 1] == ':')
		argument.pop_back();

	for (ConsoleSwitch consoleSwitch : consoleSwitches)
	{
		if (consoleSwitch.argumentMatch(argument))
		{
			currentSwitch = consoleSwitch;
			return true;
		}
	}

	return false;
}

ConsoleArgs ConsoleArgumentParser::sortArguments(int argc, char **argv)
{
	ConsoleArgs consoleArgs;

	if (argc < 2)
	{
		consoleArgs.renderSettings.helpRequested = true;
		return consoleArgs;
	}

	ConsoleSwitch currentSwitch;

	for (int i = 1; i < argc; ++i)
	{
		bool switchFound = findSwitch(argv[i], currentSwitch);

		if (switchFound)
		{
			checkForMissingSwitchArgument(i, argc, currentSwitch.numberOfArgs);

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

	return consoleArgs;
}

ImageFormat ConsoleArgumentParser::parseImageTypeToEnum(const std::string& imageTypeString)
{
	string imageTypeStringUpper = StringHelper::convertToUpper(imageTypeString);

	if (imageTypeStringUpper == "PNG")
		return ImageFormat::PNG;

	if (imageTypeStringUpper == "JPG" || imageTypeStringUpper == "JPEG")
		return ImageFormat::JPG;

	if (imageTypeStringUpper == "BMP" || imageTypeStringUpper == "BITMAP")
		return ImageFormat::BMP;

	throw runtime_error("Unable to determine final render file type. Try PNG, JPG, or BMP.");
}

bool ConsoleArgumentParser::parseBool(const string& str)
{
	string upperStr = StringHelper::convertToUpper(str);

	if (upperStr == "TRUE" || upperStr == "YES")
		return true;

	if (upperStr == "FALSE" || upperStr == "NO")
		return false;

	throw invalid_argument("Unable to parse argument into a valid boolean (True/False).");
}

void ConsoleArgumentParser::checkForMissingSwitchArgument(int index, int argc, int numberOfArgsToPass)
{
	if (index + numberOfArgsToPass >= argc)
		throw runtime_error("Missing the final argument for the supplied switch.");
}

void ConsoleArgumentParser::parseHelp(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.helpRequested = true;
}

void ConsoleArgumentParser::parseQuiet(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.quiet = true;
}

void ConsoleArgumentParser::parseOverwrite(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.overwrite = true;
}

void ConsoleArgumentParser::parseAccessArchives(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.accessArchives = false;
}

void ConsoleArgumentParser::parseScale(const char* value, ConsoleArgs& consoleArgs)
{
	// stoi will throw an exception if it is unable to parse the string into an integer
	int scaleFactor = stoi(value);

	if (scaleFactor <= 0)
		throw runtime_error("Scale Factor was set improperly.");

	consoleArgs.renderSettings.scaleFactor = scaleFactor;
}

void ConsoleArgumentParser::parseDestDirectory(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.destDirectory = value;
}

void ConsoleArgumentParser::parseImageFormat(const char* value, ConsoleArgs& consoleArgs)
{
	consoleArgs.renderSettings.imageFormat = parseImageTypeToEnum(value);
}
