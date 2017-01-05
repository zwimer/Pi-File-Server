#include "echo.hpp"

//Constructor
echo::echo() : AbstractCommand<echo>() {}

//Destructor
echo::~echo() {}

//Run the command
std::string echo::execute(std::string& args, std::string&) {
	return (args+='\n');
};

