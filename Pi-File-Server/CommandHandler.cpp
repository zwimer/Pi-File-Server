#include "CommandHandler.hpp"
#include "CHException.hpp"


//Map command names to objects
std::map<std::string, const AbstractCommand*> CommandHandler::cmds;


//Interpret and execute the command given
void CommandHandler::runCmd(const std::string& buf, const std::string thePath ) {

	//Ignore prepended whitespace
	int i; for(i = 0; i < buf.size(); i++)
		if (!isspace(buf[i])) break;

	//Get the first word of buf
	int k = i; for(; k < buf.size(); k++)
		if (isspace(buf[k])) break;

	//If there is no command, throw an exception
	if (k == buf.size()) throw CHException( CHException::NO_COMMAND );

	//Create the command name
	std::string theCmd(buf, i, k - i);

	//If it is invalud, throw an exception
	if (cmds.find(theCmd) == cmds.end())
		throw CHException( CHException::INVALID_COMMAND );

	//Create the command
	std::string args(buf, k + 1, buf.size() - k);
	auto * cmd = cmds[theCmd]->createNew(args);

	//Execute the command
	cmd->execute( thePath );

	//Prevent leaks
	delete cmd;
}

//Create a map linking commands to their names
void CommandHandler::setup() {
	for(int i = 0; i < AutoGen::numCmds; i++)
		cmds[AutoGen::cmdNames[i]] = AutoGen::cmdsArr[i];
}

