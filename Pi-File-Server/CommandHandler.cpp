#include "CommandHandler.hpp"
#include "Server.hpp"

#include <memory>
#include <thread>

//For readability
using namespace std;

//Map command names to objects
map<string, const Cmd*> CommandHandler::cmds;


//Create a map linking commands to their names
void CommandHandler::setup() { RUN_ONCE
	for(int i = 0; i < AutoGen::numCmds; i++)
		cmds[AutoGen::cmdNames[i]] = AutoGen::cmdsArr[i];
}

//Interpret and execute the command given
string CommandHandler::runCmd( std::string theCmd, std::string buf, bool newThread ) {

	//If the command doesn't exist, return so
	if (cmds.find(theCmd) == cmds.end()) {
		sstr s; s << "Error, command " << theCmd << " doesn't exist.\n";
		return s.str();
	}

	//If threading is needed
	if (newThread) {

		//Restart this function as a thread
		thread t( runCmd, std::move(theCmd), std::move(buf), false );
		t.detach();
		
		//Make return message and return it
		sstr s; s << "Command " << theCmd << " started on thread " 
			<< t.get_id() << ".\n";
		return s.str();
	}

	//Run the command
	string pth = Server::getPath();
	unique_ptr<Cmd> cmd(cmds[theCmd]->createNew());
	string ret = cmd->execute( buf, pth );

	//Alter the path and return ret
	Server::setPath( pth );
	return ret;
}

