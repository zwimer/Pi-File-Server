#include "Synchronized.hpp"
#include "FileHandler.hpp"

//Initalize static variables
const int Synchronized::CHILD = 0;
const int Synchronized::PARENT = 1;

//Used to fork, and set up comminucation
int Synchronized::smartFork() {

	//Local variables
	int ret, arr[2];
	
	//Create the pipe and fork
 	Assert(!pipe(arr), "pipe() failed");
	Assert((rt = fork()) != -1, "fork() failed");
	ret = rt ? PARENT : CHILD;

	//If parent
	if ( ret == PARENT ) {
		pthread_t t;
		pthread_create(&t, NULL, FileHandler::newChild, (void*)arr);
	}
	
	//If child
	else FileHandler::setParent(arr[1]);

	//Return
	return ret; 
}

//Used to log an action
void log(const std::string s) { 

	//Check use
	Assert(s.size(), "trying to log an empty string");
	Assert(s.find('\n') == std::string::npos || s.find('\n') == (s.size()-1), 
			"trying to log a string with a new line");

	//Format string
	std::stringstream ss;
	ss << me() << s << ((s[s.size()-1] == '\n') ? "":"\n");

	//Log the string
	FileHandler::write(FileHandler::logFile, s); 
	FileHandler::finishWriting(FileHandler::logFile); 
}

