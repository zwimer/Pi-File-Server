#include "Server.hpp"
#include "FileHandler.hpp"
#include "CommandHandler.hpp"

#include <memory>

//Define constants
const int Server::BUFFER_SIZE = 16384; 

//Define static
std::mutex Server::m;
std::string Server::thePath = "";

//Constructor
Server::Server(int s) : sock(s) { RUN_ONCE
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

//Get the current path
std::string Server::getPath() {
	std::lock_guard<std::mutex> m2(m);
	auto ret = thePath; return ret;
}

//Change the path, if it changed, log it
void Server::setPath(const std::string& s) {
	std::lock_guard<std::mutex> m2(m);
	if (s != thePath) {
		sstr s2; s2 << "Path changed to \"" << s << "\"";
		FileHandler::log(s2); thePath = s; 
	}
}


//Log and send a string
inline void respond(int sock, std::string&& s) {
	FileHandler::log(s);
	Assert(send( sock, s.data(), s.size(), 0) == s.size(), "send() failed.");
}

//The function that runs the server
void Server::start() { RUN_ONCE

	//Local variables
	bool newThread;
	int i, k;

	//Create a buffer
	char buf[BUFFER_SIZE+1]; buf[BUFFER_SIZE] = 0;
	
	//Loop and recieve data from the client
	for(int n; (n = (int)recv( sock, buf, BUFFER_SIZE, 0 ) ); ) {

		//Check for errors or disconnects
		if (n < 0) Err("recv() failed");
		else if (!n) return;
		else buf[n] = 0;

		//Check if threading should be done
		i = 0; if (buf[0] == '&') i++;
		newThread = i;

		//Ignore prepended whitespace
		for(; i < n; i++) if (buf[i] && !isspace(buf[i])) break;

		//If an empty string was recieved, note so then continue
		if (i == n) {
			respond( sock, "Empty string recieved\n" );
			//TODO
			continue;
		}

		//Find the command
		for(k = i + 1; k < n; k++) if (!buf[k] || isspace(buf[k])) break;

		if (k == n) {
			respond( sock, "Incomplete command recieved\n" );
			//TODO
			continue;
		}

		//Create needed strings string of arguments
		std::string args(&buf[k+1], n-(k+1));
		std::string cmd(&buf[i], k-i);

		//Log the command, or at least part of it
		sstr s2; s2 << "Running command: " << cmd << '\n';
		respond(sock, s2.str());
		FileHandler::log(s2);

		//TODO: change, thread
		//Run the command and send the result
		std::string output = CommandHandler::runCmd( std::move(cmd), 
													 std::move(args), newThread );

		//If there is output
		if (output.size()) {

			//Log it
			sstr s; s << "Server sent: " << output;
			FileHandler::log(s);

			//Reply to the client
			Assert( send(sock, output.data(), output.size(), 0) == output.size(),
			        "send() failed." );
		}
	}
}
