#ifndef SYNCHRONIZED_HPP
#define SYNCHRONIZED_HPP

#include <string>

//To protect the global namespace
namespace Synchronized {

	//Used to fork, and set up
	//piping for comminucation
	int smartFork();

	//Used to log an action
	void log(const char * s);
	void log(const std::string& s);

	//Values that smartFork can return
	extern const int CHILD, PARENT;
}

#endif
