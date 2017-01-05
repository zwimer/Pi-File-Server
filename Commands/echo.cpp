#include "echo.hpp"

//Constructor
echo::echo() : AbstractCommand() {}

//Destructor
echo::~echo() {}

//Run the command
std::string echo::execute(std::string& args, std::string&) {
	return (args+='\n');
};

//Create another AbstractCommand of the same type
AbstractCommand * echo::createNew() const { return new echo(); };

