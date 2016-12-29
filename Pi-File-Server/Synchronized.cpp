#include "Synchronized.hpp"
#include "FileHandler.hpp"

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
LN
	FileHandler::write(FileHandler::logFile, ss.str());
}

