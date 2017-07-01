#include "ConsoleArgumentParser.h"
#include "StringHelper.h"

namespace ConsoleArgumentParser
{
	ConsoleSwitch consoleSwitches[]
	{
		ConsoleSwitch("-S", "--SCALE", parseScale, 1),
		ConsoleSwitch("-I", "--IMAGEFORMAT", parseImageFormat, 1),
		ConsoleSwitch("-D", "--DESTINATIONDIRECTORY", parseDestDirectory, 1),
		ConsoleSwitch("-H", "--HELP", parseHelp, 0),
		ConsoleSwitch("-Q", "--QUIET", parseQuiet, 0),
		ConsoleSwitch("-O", "--OVERWRITE", parseOverwrite, 0),
		ConsoleSwitch("-A", "--ACCESSARCHIVES", parseAccessArchives, 0)
	};

	bool findSwitch(char* argumentChar, ConsoleSwitch& currentSwitch)
	{
		string argument = StringHelper::convertToUpper(argumentChar);

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

	ImageFormat ParseImageTypeToEnum(const std::string& imageTypeString)
	{
		string imageTypeStringUpper = StringHelper::convertToUpper(imageTypeString);

		if (imageTypeStringUpper == "PNG")
			return ImageFormat::PNG;

		if (imageTypeStringUpper == "JPG" || imageTypeStringUpper == "JPEG")
			return ImageFormat::JPG;

		if (imageTypeStringUpper == "BMP" || imageTypeStringUpper == "BITMAP")
			return ImageFormat::BMP;

		throw exception("Unable to determine final render file type. Try PNG, JPG, or BMP.");
	}

	bool ParseBool(const string& str)
	{
		string upperStr = StringHelper::convertToUpper(str);

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
		consoleArgs.renderSettings.quiet = true;
	}

	void parseOverwrite(const char* value, ConsoleArgs& consoleArgs)
	{
		consoleArgs.renderSettings.overwrite = true;
	}

	void parseAccessArchives(const char* value, ConsoleArgs& consoleArgs)
	{
		consoleArgs.renderSettings.accessArchives = false;
	}

	void parseScale(const char* value, ConsoleArgs& consoleArgs)
	{
		// stoi will throw an exception if it is unable to parse the string into an integer
		int scaleFactor = stoi(value);

		if (scaleFactor <= 0)
			throw exception("Scale Factor was set improperly.");

		consoleArgs.renderSettings.scaleFactor = scaleFactor;
	}

	void parseDestDirectory(const char* value, ConsoleArgs& consoleArgs)
	{
		consoleArgs.renderSettings.destDirectory = value;
	}

	void parseImageFormat(const char* value, ConsoleArgs& consoleArgs)
	{
		consoleArgs.renderSettings.imageFormat = ParseImageTypeToEnum(value);
	}
}