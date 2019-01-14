#pragma once

#include "MapImager.h"
#include <string>
#include <vector>
#include <functional>

struct ConsoleArgs
{
	RenderSettings renderSettings;
	std::vector<std::string> paths;
};

class ConsoleArgumentParser
{
	struct ConsoleSwitch
	{
		ConsoleSwitch() { }

		ConsoleSwitch(const std::string& shortSwitch, const std::string& longSwitch, std::function<void(const char* value, ConsoleArgs&)> parseFunction, int numberOfArgs) :
			shortSwitch(shortSwitch),
			longSwitch(longSwitch),
			parseFunction(parseFunction),
			numberOfArgs(numberOfArgs) {}

		std::string shortSwitch;
		std::string longSwitch;
		std::function<void(const char*, ConsoleArgs&)> parseFunction;
		int numberOfArgs; // The switch statement itself does not count as an argument.

		bool ArgumentMatch(const std::string& argument) const
		{
			return argument == shortSwitch || argument == longSwitch;
		}
	};

public:
	ConsoleArgumentParser();
	ConsoleArgs SortArguments(int argc, char **argv);

private:
	std::vector<ConsoleSwitch> consoleSwitches;

	void CheckForMissingSwitchArgument(int index, int argc, int numberOfArgsToPass);
	bool FindSwitch(char* argumentChar, ConsoleSwitch& currentSwitch);
	bool ParseBool(const std::string& str);

	static ImageFormat ParseImageTypeToEnum(const std::string& imageTypeString);
	
	static bool IsTooFewArguments(int argumentCount);

	static void ParseQuiet(const char* value, ConsoleArgs& consoleArgs);
	static void ParseScale(const char* value, ConsoleArgs& consoleArgs);
	static void ParseImageFormat(const char* value, ConsoleArgs& consoleArgs);
	static void ParseDestDirectory(const char* value, ConsoleArgs& consoleArgs);
	static void ParseHelp(const char* value, ConsoleArgs& consoleArgs);
	static void ParseOverwrite(const char* value, ConsoleArgs& consoleArgs);
	static void ParseAccessArchives(const char* value, ConsoleArgs& consoleArgs);
};
