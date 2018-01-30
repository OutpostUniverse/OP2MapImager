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

		ConsoleSwitch(std::string shortSwitch, std::string longSwitch, std::function<void(const char* value, ConsoleArgs&)> parseFunction, int numberOfArgs)
		{
			this->shortSwitch = shortSwitch;
			this->longSwitch = longSwitch;
			this->parseFunction = parseFunction;
			this->numberOfArgs = numberOfArgs;
		}

		std::string shortSwitch;
		std::string longSwitch;
		std::function<void(const char*, ConsoleArgs&)> parseFunction;
		int numberOfArgs; // The switch statement itself does not count as an argument.

		bool argumentMatch(std::string argument)
		{
			return argument == shortSwitch || argument == longSwitch;
		}
	};

public:
	ConsoleArgumentParser();
	ConsoleArgs sortArguments(int argc, char **argv);

private:
	std::vector<ConsoleSwitch> consoleSwitches;

	void checkForMissingSwitchArgument(int index, int argc, int numberOfArgsToPass);
	bool findSwitch(char* argumentChar, ConsoleSwitch& currentSwitch);
	bool parseBool(const std::string& str);

	static ImageFormat parseImageTypeToEnum(const std::string& imageTypeString);

	static void parseQuiet(const char* value, ConsoleArgs& consoleArgs);
	static void parseScale(const char* value, ConsoleArgs& consoleArgs);
	static void parseImageFormat(const char* value, ConsoleArgs& consoleArgs);
	static void parseDestDirectory(const char* value, ConsoleArgs& consoleArgs);
	static void parseHelp(const char* value, ConsoleArgs& consoleArgs);
	static void parseOverwrite(const char* value, ConsoleArgs& consoleArgs);
	static void parseAccessArchives(const char* value, ConsoleArgs& consoleArgs);
};
