#include "Synchronized.hpp"
#include "FileHandler.hpp"

//Initalize static variables
const int Synchronized::CHILD = 0;
const int Synchronized::PARENT = 1;

//Used to fork, and set up comminucation
int Synchronized::smartFork(int masterPipe[]) {

	//Local variables
    int ret, arr[2];
    
	//Create the pipe and fork
 	Assert(!pipe(arr), "pipe() failed");
	Assert((ret = fork()) != -1, "fork() failed");
    ret = ret ? PARENT : CHILD;
    
	//If child, define the pipes to use
	if ( ret == CHILD ) {
        close(arr[1]); arr[1] = masterPipe[1];
		FileHandler::setParent(arr);
    }

	//Return
	return ret; 
}

//Used to log an action
void Synchronized::log(const char * s) { log(std::string(s)); }
void Synchronized::log(const std::string& s) { 

	//Check use
	Assert(s.size(), "trying to log an empty string");
	Assert(s.find('\n') == std::string::npos || s.find('\n') == (s.size()-1), 
			"trying to log a string with a new line");

	//Format string
	sstr ss; ss << me() << s << ((s[s.size()-1] == '\n') ? "":"\n");

	//Log the string
	FileHandler::write(FileHandler::logFile, ss.str());
}

