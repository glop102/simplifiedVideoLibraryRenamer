#ifndef GLOP_CONFIG
#define GLOP_CONFIG

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <iterator>


namespace GlopConfig{

	struct Settings{
		std::map<std::string,Settings> groups;
		std::map<std::string,std::string> values;
	};

	Settings ParseFile(std::string filename);
	void SaveToFile(std::string filename,Settings& settings);

}; // namespace GlopConfig

#endif