#include "utilities.hpp"

std::string getEnvironmentVariable(std::string env, std::string defVal) {

	if(env.empty())
		return defVal;

	const char* val = std::getenv(env.c_str());
	std::string buf;
	if(NULL == val)
	{
		buf = defVal;
		return buf;
	}
	buf = val;
	return buf;
}

std::string toUpper(std::string x) {
	std::transform(x.begin(), x.end(), x.begin(), ::toupper);
	return x;
}
