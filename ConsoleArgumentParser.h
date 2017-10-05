#pragma once

#include "MapImager.h"
#include <string>
#include <vector>
#include <functional>

namespace ConsoleArgumentParser
{
	struct ConsoleArgs
	{
		RenderSettings renderSettings;
		std::vector<std::string> paths;
	};

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

	ConsoleArgs sortArguments(int argc, char **argv);

	void checkForMissingSwitchArgument(int index, int argc, int numberOfArgsToPass);
	bool findSwitch(char* argumentChar, ConsoleSwitch& currentSwitch);
	ImageFormat parseImageTypeToEnum(const std::string& imageTypeString);

	void parseQuiet(const char* value, ConsoleArgs& consoleArgs);
	void parseScale(const char* value, ConsoleArgs& consoleArgs);
	void parseImageFormat(const char* value, ConsoleArgs& consoleArgs);
	void parseDestDirectory(const char* value, ConsoleArgs& consoleArgs);
	void parseHelp(const char* value, ConsoleArgs& consoleArgs);
	void parseOverwrite(const char* value, ConsoleArgs& consoleArgs);
	void parseAccessArchives(const char* value, ConsoleArgs& consoleArgs);
}