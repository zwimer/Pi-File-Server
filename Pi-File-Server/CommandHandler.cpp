#include "CommandHandler.hpp"


//Map command names to objects
std::map<std::string, const AbstractCommand*> CommandHandler::cmds;


//Interpret and execute the command given
void CommandHandler::runCmd(const std::string& buf) {



//TODO
}


//Create a map linking commands to their names
void CommandHandler::setup() {
	for(int i = 0; i < AutoGen::numCmds; i++)
		cmds[AutoGen::cmdNames[i]] = AutoGen::cmdsArr[i];
}

