#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include "../build/AutoGen.hpp"
#include "AbstractCommand.hpp"
#include "main.hpp"

#include <map>

//A static class used to run the 
//appropriate command given some input
class CommandHandler {
public:

	//Create the map below
	static void setup();

	//Interpret and execute the command given
	static void runCmd(const std::string& buf, const std::string thePath );

	//Map command names to objects
	static std::map<std::string, const AbstractCommand*> cmds;

};

#endif
