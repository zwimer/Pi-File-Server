#ifndef SYNCHRONIZED_HPP
#define SYNCHRONIZED_HPP

#include <string>
#include <vector>

//To protect the global namespace
namespace Synchronized {

    //Static memory for pipes
    static std::vector<int> writePipes;
    
	//Used to fork, and set up
	//piping for comminucation
	int smartFork(int masterPipe[]);

	//Used to log an action
	void log(const char * s);
	void log(const std::string& s);

	//Values that smartFork can return
	extern const int CHILD, PARENT;
}

#endif
