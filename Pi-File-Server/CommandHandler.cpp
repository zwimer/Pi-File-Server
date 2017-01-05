#include "CommandHandler.hpp"

#include <memory>
#include <thread>

//For readability
using namespace std;

//Map command names to objects
map<string, const Cmd*> CommandHandler::cmds;

//Interpret and execute the command given
string CommandHandler::runCmd( string theCmd, string buf,
                               string pth, const bool newThread ) {

	//If the command doesn't exist, return so
	if (cmds.find(theCmd) == cmds.end()) {
		sstr s; s << "Error, command " << theCmd << " doesn't exist.\n";
		return s.str();
	}

	//If threading is needed
	if (newThread) {

		//Restart this function as a thread
		thread t( runCmd, std::move(theCmd), std::move(buf), std::move(pth), false );
		t.detach();
		
		//Make return message and return it
		sstr s; s << "Command " << theCmd << " started on thread " 
			<< t.get_id() << ".\n";
		return s.str();
	}

	//Execute the command and return the result
	unique_ptr<Cmd> cmd(cmds[theCmd]->createNew());
	return cmd->execute( buf, pth );
}

//Create a map linking commands to their names
void CommandHandler::setup() {
	for(int i = 0; i < AutoGen::numCmds; i++)
		cmds[AutoGen::cmdNames[i]] = AutoGen::cmdsArr[i];
}

