#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include "../build/AutoGen.hpp"
#include "Cmd.hpp"

#include <map>

//Forward declarations
class Server;

//A static class used to run the 
//appropriate command given some input
class CommandHandler {
public:

	//Create the map below
	//Called only once, before any client connects
	static void setup();

	//Set's the server this class will use
	//Each process will call this on their own server
	static void setServer(Server * s);

	//Interpret and execute the command given
	static std::string runCmd( std::string theCmd, std::string buf, bool newThread );

private:

	//To change the path
	static Server * svr;

	//Map command names to objects
	static std::map<std::string, const Cmd*> cmds;
};

#endif
