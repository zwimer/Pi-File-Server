#ifndef SYNCHRONIZED_HPP
#define SYNCHRONIZED_HPP

#include <string>
#include <sstream>

//To protect the global namespace
namespace Synchronized {

	//Used to log an action
	void log(const char * s);
	void log(const std::string& s);
	void log(const std::stringstream& s);
}

#endif
