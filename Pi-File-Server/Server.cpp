#include "Server.hpp"
#include "FileHandler.hpp"

//For ease of readability
using namespace Synchronized;

//Define constants
const int Server::BUFFER_SIZE = 16384; 

//Constructor
Server::Server(int s) : sock(s) {
    sstr s2; s2 << "Child server has started.";
    log(s2);
    start();
}

//Destructor
Server::~Server() {
    sstr s2; s2 << "Client disconnected from server.";
	FileHandler::userQuit(me());
	log(s2);
}

//The function that runs the server
void Server::start() {

	//Create a buffer
	char buffer[BUFFER_SIZE+1]; buffer[BUFFER_SIZE] = 0;
    
	//Loop and recieve data from the client
	for(int n; (n = (int)recv( sock, buffer, BUFFER_SIZE, 0 ) ); ) {

		//Check for errors or disconnects
        if (n < 0) Err("recv() failed");
		else if (!n) return;

		//Log the command, or at least part of it
		log(std::string(buffer, 0, min(n, COMMAND_MAX_LEN)));

		log("THIS WILL BE IMPLEMENTED WHERE IT READS THE COMMANDS THEN LOOKS THROUGH THAT FOLDER");

		std::string f = "TestFile", c = "This is a test!";
		sstr s; s << "Wrote '" << c << "' to file: " << f; log(s);
		FileHandler::overWrite(f,c);

	}
}
