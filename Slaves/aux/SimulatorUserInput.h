#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include "stringhelper.h"


// Modifica std::cin!!
class SimulatorUserInput
{
public:
	void simulate(std::string in)
	{
		std::vector<std::string> lines = split(in, '\n');
		reverse(lines.begin(), lines.end());

		for(std::string line: lines)
			simulateLine(line);
	}

	void simulateLine(std::string line)
	{
		line += '\n';
		reverse(line.begin(), line.end());
		for(char c: line)
			std::cin.putback(c);
	}
};