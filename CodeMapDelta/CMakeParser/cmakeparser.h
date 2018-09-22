#ifndef CMAKEPARSER_H
#define CMAKEPARSER_H

#include <string>
#include <vector>

namespace cm {

class CMakeParser
{
public:
	static std::vector<std::string> parseCmakeIncludes();

};

};

#endif //CMAKEPARSER_H