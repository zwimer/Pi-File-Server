#include "echo.hpp"

//Print the arguments given
std::string echo::execute(std::string& args, std::string&) {
	return (args+='\n');
};

