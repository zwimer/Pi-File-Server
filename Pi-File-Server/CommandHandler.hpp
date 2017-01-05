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
	static std::string runCmd( std::string theCmd, std::string buf,
	                           std::string pth, const bool newThread );

private:

	//Map command names to objects
	static std::map<std::string, const Cmd*> cmds;

};

#endif
