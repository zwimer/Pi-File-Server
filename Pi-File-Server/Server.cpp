#include "Server.hpp"
#include "FileHandler.hpp"
#include "CommandHandler.hpp"

#include <memory>

//Define constants
const int Server::BUFFER_SIZE = 16384; 


//Constructor
Server::Server(int s) : sock(s) {
	sstr s2; s2 << "Child server has started.";
	FileHandler::log(s2);
	start();
}

//Destructor
Server::~Server() {
	sstr s2; s2 << "Client disconnected from server.";
	FileHandler::userQuit(me());
	FileHandler::log(s2);
}


//Log and send a string
inline void respond(int sock, const std::string& s, const bool addNewL = true) {

	//Add newline if wanted
	string s2 = s; if(addNewL) s2 += '\n';

	//Log the message
	FileHandler::log(s2);

	//Send the string
	Assert(send( sock, s2.data(), s2.size(), 0) == s2.size(),
		"send() failed.");
}

//The function that runs the server
void Server::start() {

	//Create a buffer
	char buf[BUFFER_SIZE+1]; buf[BUFFER_SIZE] = 0;
	
	//Loop and recieve data from the client
	for(int n; (n = (int)recv( sock, buf, BUFFER_SIZE, 0 ) ); ) {

		//Check for errors or disconnects
		if (n < 0) Err("recv() failed");
		else if (!n) return;
		else buf[n] = 0;

		//Ignore prepended whitespace
		int i; for(i = 0; i < n; i++)
			if (buf[i] && !isspace(buf[i])) break;

		//If an empty string was recieved, note so then continue
		if (i == n) {
			respond( sock, "Empty string recieved" );
			//TODO
			continue;
		}

		//Find the command
		int k; for(k = i + 1; k < n; k++)
			if (!buf[k] || isspace(buf[k])) break;

		if (k == n) {
			respond( sock, "Incomplete command recieved" );
			//TODO
			continue;
		}

		//Create needed strings string of arguments
		std::string args(&buf[k+1], n-(k+1));
		std::string cmd(&buf[i], k-i);

		//Log the command, or at least part of it
		sstr s2; s2 << "Running command: " << cmd;
		respond(sock, s2.str());
		FileHandler::log(s2);

		//TODO: change, thePath, thread
		std::string pth = "";

		//TODO: change, thePath, thread
		//Run the command and send the result
		respond(sock, CommandHandler::runCmd( std::move(cmd), std::move(args), 
		                                      std::move(pth), false), false );
	}
}
